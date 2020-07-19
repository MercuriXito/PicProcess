#include <exception>
#include <QDebug>

#include "arithmops.h"
#include "wimage.h"
#include "lib/libcaller.h"

#define debug(x)( qDebug() << (x))
#define max(x, y)( (x) > (y) ? (x):(y))
#define min(x, y)( (x) < (y) ? (x):(y))

ArithmOps::ArithmOps()
{

}

//typedef struct info{
//    bool doChannel;
//    bool doSize;
//    bool doReSize;
//    bool doMaxSize;
//    bool doCenterAligned;
//    int cx1, cy1, cx2, cy2, oWidth, oHeight;
//    bool success;
//    info():
//        doChannel(false), doSize(false), doReSize(false), doMaxSize(false),
//        doCenterAligned(false), cx1(0),cy1(0),cx2(0),cy2(0),oWidth(0),oHeight(0),
//        success(false){}
//}Info;

inline QRgb Pixel(const WImage& image, int x, int y){
    if(x < 0 || x >= image.width() || y < 0 || y >= image.height() ) return 0;
    return image.pixel(x, y);
}

inline QRgb Pixel(const QImage& image, int x, int y){
    if(x < 0 || x >= image.width() || y < 0 || y >= image.height() ) return 0;
    return image.pixel(x, y);
}

inline int clip0256(int x){
    return min(255, max(x, 0));
}

inline int mod0256(int x){
    return (x + 255) % 255;
}

inline int perPixelAdd(int a, int b){return a + b;}
inline int perPixelSub(int a, int b){return a - b;}
inline int perPixelMul(int a, int b){return a * b;}
inline int perPixelDiv(int a, int b){ if(b == 0) return 255; return a / b; }
inline int perPixelAnd(int a, int b){
    if(b == 0) return 0; return a;
};
inline int perPixelOr(int a, int b){
    if(b > 0) return a; return 0;
};

// flag related
uint ArithmOps::constructFlag(bool doCenterAligned, bool doScale, bool useMode){
    uint flag = 0;
    flag |= ( doCenterAligned );
    flag |= ( doScale << 1 );
    flag |= ( useMode  << 2);
    return flag;
}

void ArithmOps::destructFlag(uint flag, bool &doCenterAligned, bool &doScale, bool &useMod){
    doCenterAligned = flag & 1;
    doScale = flag & 2;
    useMod = flag & 4;
}

bool ArithmOps::calPerPixel(WImage &image, const WImage &image2, int (*fun)(int, int), uint flag){

    // 设定：
    // 0. 一切大小以image的大小为主
    // 解决 Size 不匹配的问题:
    // 1. 中心对齐，或者是左上角对齐： doCenterShift 参数控制
    // 2. image2 进行 scale: doScale 参数控制
    // 解决显示时超过255的问题：
    // 1. 两种处理的方法，一种是取模，一种是clip: 由 useMod 参数控制，默认是 clip

    bool doCenterAligned, doScale, useMod;
    ArithmOps::destructFlag(flag, doCenterAligned, doScale, useMod);

    QImage op2 = image2;
    if( doScale ){
        op2 = image2.scaled(image.width(), image.height());
        doCenterAligned = false;
    }

    int hw1 = image.width() / 2, hh1 = image.height() / 2, hw2 = op2.width() / 2, hh2 = op2.height() / 2;
    int xshift = hw2 - hw1, yshift = hh2 - hh1;

    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
                QRgb i1 = Pixel(image, i, j);
                QRgb i2 = Pixel(op2, i + xshift * doCenterAligned, j + yshift * doCenterAligned); // doCenterAligned or Not
                int r, g, b;
                if(useMod){
                    r = mod0256( fun( qRed(i1) , qRed(i2) ) ) ;
                    g = mod0256( fun( qGreen(i1) , qGreen(i2) ) ) ;
                    b = mod0256( fun( qBlue(i1) , qBlue(i2)) );
                }else{
                    r = clip0256( fun( qRed(i1) , qRed(i2) ) ) ;
                    g = clip0256( fun( qGreen(i1) , qGreen(i2) ) ) ;
                    b = clip0256( fun( qBlue(i1) , qBlue(i2)) );
                }
                image.setPixel(i,j,QColor(r, g, b).rgb());
        }
    }

    return true;
}

//Info resovleStatus(const WImage& image1, const WImage& image2, ArithmOps::ArithmOption opt){
//    uchar status =  opt & 0x000000ff;
//    uchar statusChannel = (status & 0x000000f0) >> 4;
//    uchar statusSize = status & 0x0000000f;

//    bool image1IsGrayScale = image1.isGrayscale();
//    bool image2IsGrayScale = image2.isGrayscale();

//    int outWidth = image1.width(), outHeight = image1.height();

//    Info info;
//    // check channels
//    switch (statusChannel) {
//        case 0:{
//        if(image1IsGrayScale ^ image2IsGrayScale){
//            debug("Channel Match Failed"); debug(image1IsGrayScale); debug(image2IsGrayScale);
//            return info;
//        }}; break;
//        case 1: info.doChannel = true ;break;
//    default: debug("Not Seen Channel Status:"); debug(statusChannel); return info; break;
//    }

//    // check size
//    switch (statusSize) {
//        case 0: if(!(image1.height() == image2.height() && image1.width() == image2.width())){
//            debug("Size Match Failed");
//            debug(image1.height()); debug(image2.height()); debug(image1.width()); debug(image2.width());
//            return info;
//        }; break;
//    case 1: { // rescale image2
//        info.doSize = true; info.doReSize = true;
//    };break;
//    case 2:{ // find max
//        info.doSize = false;
//        info.doMaxSize = true; outWidth = max(image1.width(), image2.width()); outHeight = max(image1.height(), image2.height());}
//    case 3:{
//        info.doSize = false;
//        info.doMaxSize = true; outWidth = max(image1.width(), image2.width()); outHeight = max(image1.height(), image2.height());
//        // center aligned during adding
//        int hw1 = image1.width() / 2, hh1 = image1.height() / 2, hw2 = image2.width() / 2, hh2 = image2.height();
//        int hw = max(hw1, hw2), hh = max(hh1, hh2);
//        info.cx1 = hw - hw1; info.cx2 = hw - hw2; info.cy1 = hh - hh1; info.cy2 = hh - hh2;
//    };break;
//    default: debug("Not Seen Size Status:"); debug(statusSize); return info; break;
//    }
//    info.oWidth = outWidth; info.oHeight = outHeight;

//    info.success = true;
//    return info;
//}



bool ArithmOps::add(WImage& image , const WImage& image2, uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelAdd, flag);
}

WImage ArithmOps::add(const WImage& image, const WImage& image2 , uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::add(*res, image2, flag);
    return *res;
}

bool ArithmOps::minus(WImage& image , const WImage& image2, uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelSub, flag);
}

WImage ArithmOps::minus(const WImage& image, const WImage& image2, uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::minus(*res, image2, flag);
    return *res;
}

bool ArithmOps::mul(WImage& image , const WImage& image2, uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelMul, flag);
}

WImage ArithmOps::mul(const WImage& image , const WImage& image2, uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::mul(*res, image2, flag);
    return *res;
}

bool ArithmOps::div(WImage& image , const WImage& image2, uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelDiv, flag);
}

WImage ArithmOps::div(const WImage& image, const WImage& image2, uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::div(*res, image2, flag);
    return *res;
}


// logicstic
bool ArithmOps::neg(WImage& image){
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            image.setPixel(i, j, QColor(255 - r, 255 - g, 255 - b).rgb());
        }
    }
    return true;
}

WImage ArithmOps::neg(const WImage& image){
    WImage res = *image.deepcopy();
    ArithmOps::neg(res);
    return res;
}

bool ArithmOps::orI(WImage& image, const WImage& image2, uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelAdd, flag);
}

WImage ArithmOps::orI(const WImage& image, const WImage& image2 , uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::orI(*res, image2, flag);
    return *res;
}

bool ArithmOps::andI(WImage& image , const WImage& image2 , uint flag){
    return ArithmOps::calPerPixel(image, image2, perPixelOr, flag);
}

WImage ArithmOps::andI(const WImage& image, const WImage& image2 , uint flag){
    WImage* res = image.deepcopy();
    ArithmOps::andI(*res, image2, flag);
    return *res;
}

