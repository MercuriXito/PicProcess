#include "ops.h"

#include "wimage.h"
#include "lib/libcaller.h"

Ops::Ops()
{

}

void Ops::pixelModify(WImage &image, unsigned int (*func)(int, int, int)){
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            int res = func(r, g, b);
            image.setPixel(i, j, QColor(res, res, res).rgb());
        }
    }
}

void Ops::pixelModify(WImage& image, void (*func)(int, int, int, int&, int&, int&)){
    int tr, tg, tb;
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            func(r, g, b, tr, tg, tb);
            image.setPixel(i, j, QColor(tr, tg, tb).rgb());
        }
    }
}
