#include <complex>
#include <QDebug>

#include "fftops.h"
#include "wimage.h"

#define debug(x)(qDebug() << (x))

const float pi = 3.1415926;

using namespace std;

FFTOps::FFTOps()
{

}

template<typename T>
void swapE(T& a, T& b){
    T c; c = a; a = b; b = c;
}

inline QRgb Pixel(const WImage& image, int x, int y){
    if(x < 0 || y < 0 || x >= image.width() || y >= image.height()) return 0;
    return image.pixel(x, y);
}

/* transpose non-square matrix */
inline int getNext(int i, int m, int n){
    return (i%n) * m + i / n;
}

inline int getPre(int i, int m, int n){
    return (i%m) * n + 	i / m;
}

inline void moveData(Complex* pic, int i, int m, int n){
    Complex temp = pic[i];
    int cur = i;
    int pre = getPre(cur, m, n);
    while(pre != i){
        pic[cur] = pic[pre];
        cur = pre;
        pre = getPre(cur, m, n);
    }
    pic[cur] = temp;
}

inline void transposeNSM(Complex* pic, int m, int n){
    for(int i = 0; i < m * n; i ++){
        int next = getNext(i, m, n);
        while(next > i) next = getNext(next, m, n);
        if(next == i) moveData(pic, i, m, n);
    }
}

// 直接按照DFT公式计算: O(n^2)，但是n可以取任意值
// 主要用来做项数重构...
void FFTOps::dft1d(Complex* a, int len, bool inverse, int truncation){
    Complex* res = new Complex[len];
    int fuhao = 2*inverse - 1;
    if(truncation <= 0){
        truncation = len;
    }
    Complex wn(cos(2 * pi / len), sin(fuhao * 2*pi / len));
    Complex* w = new Complex[len];
    w[0] = Complex(1, 0);
    for(int i = 1; i < len ; i ++){
        w[i] = w[i-1] * wn;
    }
    for(int i = 0; i < len ; i ++){
        Complex sum(0, 0);
        for(int j = 0; j < truncation; j ++){
            sum += a[j] * w[(j * i) % len];
        }
        res[i] = sum;
    }
    for(int i = 0; i < len ; i ++){
        if(inverse) a[i] = res[i] / Complex(len, 0);
        else a[i] = res[i];
    }

    delete[] w;
    delete[] res;
}

// 二分递归的 fft1d 实现, n 必须是二次幂
void FFTOps::fft1dRecur(Complex* a , int len){
    if(len == 1)
        return;
    Complex* a0 = new Complex[len];
    Complex* a1 = new Complex[len];

    for(int i = 0; i < (len / 2); i ++){
        a0[i] = a[2*i];
        a1[i] = a[2*i + 1];
    }
    fft1dRecur(a0, len / 2);
    fft1dRecur(a1, len / 2);

    Complex wn(cos(2 * pi / len), sin(- 2*pi / len));
    Complex wm(1,0);
    for(int i = 0; i < (len / 2); i ++){
        a[i] = a0[i] + wm * a1[i] ;
        a[i + len / 2] = a0[i] - wm * a1[i];
        wm *= wn;
    }
    delete []a0;
    delete []a1;
}

// 前向的蝶式计算，先重排再计算，n 必须是二次幂
void FFTOps::FFTOps::fft1dBf(Complex* a, int len, bool inverse = false){
    int log2n = int(log2(len));
    // permute
    int* tab = new int[len];
    tab[0] = 0;
    for(int i = 0; i < len; i ++){
        tab[i] = ((tab[i >> 1] >> 1) | ((i & 1) << (log2n - 1)));
    }
    for(int i = 0; i < len; i ++){
        if(i > tab[i]) continue;
        std::swap(a[i], a[tab[i]]);
    }
    int fuhao = inverse * 2 - 1;
    for(int l = 1; l < len; l <<= 1){
        int step = ( l << 1);
        Complex wn(cos(2 * pi / step), sin(fuhao * 2* pi/ step));
        for(int g = 0; g < len; g += step){
            Complex wm(1, 0);
            for(int i = 0; i < l; i ++, wm = wm * wn ){
                Complex a1 = a[g + i];
                Complex a2 = a[g + l + i] * wm;
                a[g + i] = a1 + a2;
                a[g + l + i] = a1 - a2;
            }
        }
    }

    delete []tab;
}

void FFTOps::FFTOps::ifft1dBf(Complex* a, int len){
    FFTOps::fft1dBf(a, len, true);
    for(int i = 0; i < len; i ++){
       a[i] /= Complex(len, 0);
    }
}


void FFTOps::fft2dGray(WImage &image, bool doCenterShift, bool doCompress){
    // 仅针对灰度图
    int outWidth = image.width();
    int outHeight = image.height();

    bool doPadZero = false; // 不足补0，空域补0相当于频域的插值
    float log2w = log2(outWidth);
    float log2h = log2(outHeight);

    if(log2w - int(log2w) > 0){
        outWidth = int(pow(2, int(log2w) + 1));
    }
    if(log2h - int(log2h) > 0){
        outHeight = int(pow(2, int(log2h) + 1));
    }

    Complex* pic = new Complex[outWidth * outHeight];
    debug(" pic ");

    // res 赋值，对非二次幂的图像，扩展到二次幂大小，并且补0
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = Pixel(image, i, j);
            int r = qRed(rgb);
            pic[i * outHeight + j] = Complex(r * int(pow( -1, (i + j) * doCenterShift)), 0);
        }
    }

    FFTOps::fft2d(pic, outWidth, outHeight);

    // 转化为幅值图，并动态压缩，其实不动态压缩也没办法在[0,255]内显示？
    Complex umax = Complex(0,0);
    for(int i = 0; i < outHeight; ++i){
        for(int j = 0; j < outWidth; ++j){
            double abst = std::abs(pic[i * outWidth + j]);
            if(doCompress){
                abst = log(abst) + 1;
            }
            pic[i * outWidth + j] = Complex(abst,0);
            if(std::real(pic[i * outWidth + j]) > std::real(umax)) umax = pic[i * outWidth + j];
        }
    }
    debug(outWidth);
    debug(outHeight);

    // 替换赋值，非二次幂大小的图像是补零成二次幂大小计算的，FFT的结果还是二次幂大小的图像，
    // 这里为了显示的需要对二次幂图像，将结果 resize 到原来的大小了。
    QImage res(outWidth, outHeight, QImage::Format_RGB32);
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            int color = int( std::real(pic[j * outWidth + i]) / std::real(umax) * 255);
            res.setPixel(i, j, QColor(color, color, color).rgb());
        }
    }

//    QImage teres = res.scaled(image.width(), image.height());
    QImage teres = res;
    for(int i = 0; i < image.width(); ++i){
        for(int j = 0; j < image.height(); ++j){
            int color = qRed(teres.pixel(i, j));
            image.setPixel(i, j, QColor(color, color, color).rgb());
        }
    }

    delete []pic;
}

void FFTOps::fft2d(Complex* pic, int outWidth, int outHeight){
    // y 方向 1d-fft
    for(int i = 0; i < outWidth; i ++){
        FFTOps::fft1dBf(pic + outHeight * i, outHeight);
    }
    // transpose
    if(outWidth == outHeight){ // 方阵 transpose
        for(int i = 0; i < outWidth; ++i){
            for(int j = i + 1; j < outHeight; ++j){
                std::swap(pic[j * outHeight + i], pic[i * outWidth + j]);
            }
        }
    }else{ // 非方阵的 transpose
        transposeNSM(pic, outWidth, outHeight);
    }
    // x 方向 1d-fft
    for(int i = 0; i < outHeight; i ++){
        FFTOps::fft1dBf(pic + outWidth * i, outWidth);
    }
}

void FFTOps::ifft2d(Complex* pic, int outWidth, int outHeight){
    // pic 已经存储了 fft2d 的数值
    // y 方向逆fft-1d
    for(int i = 0; i < outHeight; i ++){
        FFTOps::ifft1dBf(pic + outWidth * i, outWidth);
    }
    // transpose
    if(outWidth == outHeight){ // 方阵 transpose
        for(int i = 0; i < outWidth; ++i){
            for(int j = i + 1; j < outHeight; ++j){
                std::swap(pic[j * outHeight + i], pic[i * outWidth + j]);
            }
        }
    }else{ // 非方阵的 transpose
        transposeNSM(pic, outHeight, outWidth); // 这里长宽有点意思
    }
    // x 方向逆 fft-1d
    for(int i = 0; i < outWidth; i ++){
        FFTOps:ifft1dBf(pic + outHeight * i , outHeight);
    }
}

WImage FFTOps::fft2d(const WImage& image){

}

void FFTOps::fft2dThenIfft2d(WImage& image){

    // TODO: 不管是高长还是宽长，现在都没办法正常显示
    // 仅针对灰度图
    int outWidth = image.width();
    int outHeight = image.height();

    bool doPadZero = false; // 不足补0，空域补0相当于频域的插值
    float log2w = log2(outWidth);
    float log2h = log2(outHeight);

    if(log2w - int(log2w) > 0){
        outWidth = int(pow(2, int(log2w) + 1));
    }
    if(log2h - int(log2h) > 0){
        outHeight = int(pow(2, int(log2h) + 1));
    }

    Complex* pic = new Complex[outWidth * outHeight];
    debug(" pic ");

    bool doCenterShift = true;

    // res 赋值，对非二次幂的图像，扩展到二次幂大小，并且补0
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = Pixel(image, i, j);
            int r = qRed(rgb);
            pic[i * outHeight + j] = Complex(r * int(pow( -1, (i + j) * doCenterShift)), 0);
        }
    }

    FFTOps::fft2d(pic, outWidth, outHeight);
    FFTOps::ifft2d(pic, outWidth, outHeight);
    for(int i = 0; i < image.width(); ++i){
        for(int j = 0; j < image.height(); ++j){
            int color =  int(std::abs(pic[i * outHeight + j]));
            image.setPixel(i, j, QColor(color, color, color).rgb());
        }
    }

    delete []pic;
}
