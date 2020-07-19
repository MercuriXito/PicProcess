#include "basicops.h"

#include "wimage.h"

#define min(x, y)( (x) < (y) ? (x) : (y) )

const double pi = 3.1415926;

BasicOps::BasicOps()
{

}

inline unsigned int average(int a, int b, int c){
    return (a + b + c) / 3;
}

void BasicOps::toGrayScale(WImage& image){
    BasicOps::pixelModify(image, average);
}

WImage BasicOps::toGrayScale(const WImage& image){
    WImage res = *image.deepcopy();
    BasicOps::toGrayScale(res);
    return res;
}

void BasicOps::toBinary(WImage& image, int threshold = 128){
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            int avg = (r + g + b) / 3;
            int res = (avg > threshold) * 255;
            image.setPixel(i, j, QColor(res, res, res).rgb());
        }
    }
}

WImage BasicOps::toBinary(const WImage& image, int threshold = 128){
    WImage res = *image.deepcopy();
    BasicOps::toBinary(res, threshold);
    return res;
}

HSIImage BasicOps::rgb2hsv(const WImage &image){
    int outWidth = image.width();
    int outHeight = image.height();
    HSIImage res(outWidth, outHeight, true);
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            double i = ( r + g + b ) / ( 3 * 255);
            double s = 1 - ( (min(r,g) , b) / float(i));
            double f = (2*r - g - b) / float(g - b);
            double h = ( 90 - (atan( f / sqrt(3)) / pi * 360) + (g < b) * 180 ) / float(360);
            res(i,j) = HsiPixel(h, s, i);
        }
    }
    return res;
}

WImage BasicOps::hsv2rgb(const HSIImage &image){
    int outWidth = image.width();
    int outHeight = image.height();
    WImage res(outWidth, outHeight);

    //TODO: left

    return res;
}
