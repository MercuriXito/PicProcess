#include "filterops.h"

#include <cmath>
#include <queue>
#include <QQueue>
#include <QDebug>
#include <QProgressDialog>

#include "wimage.h"
#include "lib/ops/arithmops.h"

const double pi = 3.14159268;

FilterOps::FilterOps()
{

}

// auxiliary functions

inline QRgb Pixel(const WImage& image, int x, int y){
    if(x < 0 || y < 0 || x >= image.width() || y >= image.height()) return 0;
    return image.pixel(x, y);
}

inline double l2(double x, double y){
    return sqrt(pow(x, 2) + pow(y, 2));
}

inline double gaussian2d(int x, int y){
    return exp( - (pow(x, 2) + pow(y, 2)) / 2.0 ) / ( 2 * pi );
}

inline double l2(int x, int y){
    return sqrt(pow(double(x), 2) + pow(double(y), 2));
}

bool FilterOps::filter(const Kernel* kptr, WImage& image, int stride, PaddingMode mode, QProgressDialog* bar){

//    int count = k.width * k.height;
    Kernel k = *kptr;
    int halfw = k.width / 2;
    int halfh = k.height / 2;

    qDebug() << k.width << " " << k.height;

    double rsum = 0.0;
    double gsum = 0.0;
    double bsum = 0.0;

    QImage temp(image.width(), image.height(), QImage::Format_RGB32);

    // TODO: 重大 BUG，需要一个中间的copy
    for(int x = 0; x < image.width(); x += stride){
        for(int y = 0; y < image.height(); y += stride){
            for(int i = 0; i < k.width;  ++i){
                for(int j = 0; j < k.height ; ++j){
                    QRgb rgb = Pixel(image, x + i - halfw, y + j - halfh);
                    rsum += qRed(rgb) * k(i, j);
                    gsum += qGreen(rgb) * k(i, j);
                    bsum += qBlue(rgb) * k(i, j);
                }
            }
            temp.setPixel(x, y, QColor(abs(int(rsum)), abs(int(gsum)), abs(int(bsum))).rgb());
            rsum = 0, gsum = 0, bsum = 0;
        }
//        qDebug() << x << " " << image.width();
    }

    for(int i = 0; i < image.width(); i ++){
        for(int j = 0; j < image.height(); j ++){
            image.setPixel(i, j, temp.pixel(i, j));
        }
    }
    return true;
}

const WImage& FilterOps::filter2(const Kernel*, const WImage&, int stride, PaddingMode mode){


}

Kernel* FilterOps::meanKernel(int width, int height){
    if(height == 0){
        height = width;
    }
    int count = width * height;
    Kernel* kernel = new Kernel(width, height);
    for(std::vector<double>::iterator it = kernel->mat->begin(); it != kernel->mat->end();  it ++){
        *it = (1 / float(count));
    }
    return kernel;
}

Kernel* FilterOps::gaussianKernel(int width, int height){
    if(height == 0){
        height = width;
    }
    int cx = width / 2, cy = height / 2;
    Kernel* kernel = new Kernel(width, height);
    for(int i = 0; i < width * height; ++i){
        int x = i / width, y = i % width;
        kernel->mat->at(i) = gaussian2d(x - cx, y - cy);
    }
    return kernel;
}

std::vector<Kernel* >* FilterOps::robertsKernel(){
    std::vector<Kernel* >* roberts = new std::vector<Kernel* >(2);
    double ele1[] = {1, 0, 0, -1};
    double ele2[] = {0, 1, -1, 0};
    Kernel* k1 = new Kernel(ele1, 2, 2);
    Kernel* k2 = new Kernel(ele2, 2, 2);
    roberts->at(0) = k1;
    roberts->at(1) = k2;
    return roberts;
}

std::vector<Kernel* >* FilterOps::sobelKernel(){
    std::vector<Kernel* >* sobel = new std::vector<Kernel* >(2);
    double ele1[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    double ele2[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    Kernel* k1 = new Kernel(ele1, 3, 3);
    Kernel* k2 = new Kernel(ele2, 3, 3);
    sobel->at(0) = k1;
    sobel->at(1) = k2;
    return sobel;
}

std::vector<Kernel* >* FilterOps::prewittKernel(){
    std::vector<Kernel* >* prewitt = new std::vector<Kernel* >(2);
    double ele1[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    double ele2[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    Kernel* k1 = new Kernel(ele1, 3, 3);
    Kernel* k2 = new Kernel(ele2, 3, 3);
    prewitt->at(0) = k1;
    prewitt->at(1) = k2;
    return prewitt;
}

Kernel* FilterOps::lapLaceKernel(){
    double ele[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
    Kernel* kernel = new Kernel(ele, 3, 3);
    return kernel;
}


bool FilterOps::meanFiltered(WImage &image, int kernelWidth, int kernelHeight, QProgressDialog* bar){
    const Kernel* k = FilterOps::meanKernel(kernelWidth, kernelHeight);
    FilterOps::filter(k, image, 1, FilterOps::PADDING_ZERO, bar);

    delete k;
    k = NULL;
    return true;
}

bool FilterOps::gaussianFiltered(WImage & image, int kernelWidth, int kernelHeight, QProgressDialog* bar){
    Kernel* k = FilterOps::gaussianKernel(kernelWidth, kernelHeight);
    FilterOps::filter(k, image, 1, FilterOps::PADDING_ZERO, bar);

    delete k;
    k = NULL;
    return true;
}


bool FilterOps::medianFiltered(WImage &image , int kernelWidth, int kernelHeight, int stride, QProgressDialog* bar){
    if(kernelHeight == 0){
        kernelHeight = kernelWidth;
    }
    int halfw = kernelWidth / 2;
    int halfh = kernelHeight / 2;
    int total = image.width() * image.height();

    qDebug() << kernelWidth << " " << kernelHeight;

    std::queue<int > rque;
    std::queue<int > gque;
    std::queue<int > bque;
    std::vector<int > rvec;
    std::vector<int > gvec;
    std::vector<int > bvec;
    int midx = kernelWidth * kernelHeight / 2;

    for(int x = 0; x < image.width(); x += stride){
        for(int y = 0; y < image.height(); y += stride){

            for(int j = 0; j < kernelHeight ; ++j){
                for(int i = 0; i < kernelWidth;  ++i){
                    QRgb rgb = Pixel(image, x + i - halfw, y + j - halfh);
                    rvec.push_back(qRed(rgb));
                    gvec.push_back(qGreen(rgb));
                    bvec.push_back(qBlue(rgb));
                }
            }
            sort(rvec.begin(), rvec.end());
            sort(gvec.begin(), gvec.end());
            sort(bvec.begin(), bvec.end());

            image.setPixel(x, y, QColor(rvec[midx], gvec[midx], bvec[midx]).rgb());
            rvec.clear(); gvec.clear(); bvec.clear();
            if(bar){
                bar->setValue( int((x*image.height()+ y)/float(total) * 100) );
            }
        }
//        qDebug() << x << " " << image.width();
    }
    return true;
}

bool FilterOps::robertsDetect(WImage &image){
    image.convertToFormat(QImage::Format_Grayscale8);
//    return false;
    WImage* image2 = image.deepcopy();
    std::vector<Kernel* > * roberts = FilterOps::robertsKernel();

    FilterOps::filter(roberts->at(0), image);
    FilterOps::filter(roberts->at(1), *image2);

    for(int x = 0; x < image.width(); ++x){
        for(int y = 0; y < image.height(); ++y){
            int gradx = qRed(image.pixel(x, y));
            int grady = qRed(image2->pixel(x, y));
            int avg = int(l2(gradx, grady));
            image.setPixel(x, y, qRgb(avg, avg, avg));
        }
    }

    for(std::vector<Kernel* >::iterator it = roberts->begin(); it != roberts->end(); it ++){
        delete *it;
        *it = NULL;
    }
    delete roberts;
    roberts = NULL;
    delete image2;
    image2 = NULL;

    return true;
}

bool FilterOps::sobelDetect(WImage &image){
    image.convertToFormat(QImage::Format_Grayscale8);
    WImage* image2 = image.deepcopy();
    std::vector<Kernel* > * sobel = FilterOps::sobelKernel();

    FilterOps::filter(sobel->at(0), image);
    FilterOps::filter(sobel->at(1), *image2);

    for(int x = 0; x < image.width(); ++x){
        for(int y = 0; y < image.height(); ++y){
            int gradx = qRed(image.pixel(x, y));
            int grady = qRed(image2->pixel(x, y));
            int avg = int(l2(gradx, grady));
            image.setPixel(x, y, qRgb(avg, avg, avg));
        }
    }

    for(std::vector<Kernel* >::iterator it = sobel->begin(); it != sobel->end(); it ++){
        delete *it;
        *it = NULL;
    }
    delete sobel;
    sobel = NULL;
    delete image2;
    image2 = NULL;

    return true;
}

bool FilterOps::prewittDetect(WImage &image){
    image.convertToFormat(QImage::Format_Grayscale8);
    WImage* image2 = image.deepcopy();
    std::vector<Kernel* > * prewitt = FilterOps::prewittKernel();

    FilterOps::filter(prewitt->at(0), image);
    FilterOps::filter(prewitt->at(1), *image2);

    for(int x = 0; x < image.width(); ++x){
        for(int y = 0; y < image.height(); ++y){
            int gradx = qRed(image.pixel(x, y));
            int grady = qRed(image2->pixel(x, y));
            int avg = int(l2(gradx, grady));
            image.setPixel(x, y, qRgb(avg, avg, avg));
        }
    }

    for(std::vector<Kernel* >::iterator it = prewitt->begin(); it != prewitt->end(); it ++){
        delete *it;
        *it = NULL;
    }
    delete prewitt;
    prewitt = NULL;
    delete image2;
    image2 = NULL;

    return true;
}

bool FilterOps::laplaceDetect(WImage &image){
    image.convertToFormat(QImage::Format_Grayscale8);
    const Kernel* k = FilterOps::lapLaceKernel();
    FilterOps::filter(k, image);
    delete k;
    k = NULL;
    return true;
}

bool FilterOps::laplaceSharpen(WImage &image){

    WImage* temp = image.deepcopy();
    FilterOps::laplaceDetect(image);
    ArithmOps::add(image, *temp, ArithmOps::constructFlag(true, false, false));
    delete temp;
    return true;
}
