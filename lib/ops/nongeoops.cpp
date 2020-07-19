#include "nongeoops.h"

#include "wimage.h"

NonGeoOps::NonGeoOps()
{

}

double NonGeoOps::texp = 1.0;

inline void NonGeoOps::expScale(int r, int g, int b, int& tr, int& tg, int& tb){
    double e = texp;
    double rr = r / 255.0, rg = g / 255.0, rb = b / 255.0;
    tr = int(pow(rr, e) * 255.0);
    tg = int(pow(rg, e) * 255.0);
    tb = int(pow(rb, e) * 255.0);
}

void NonGeoOps::scaleGrayExp(WImage& image, double exp){
    texp = exp;
    Ops::pixelModify(image, NonGeoOps::expScale);
}

WImage NonGeoOps::scaleGrayExp(const WImage &image, double exp){
    WImage res = *image.deepcopy();
    NonGeoOps::scaleGrayExp(res, exp);
    return res;
}

// 单通道的直方图均衡化, 现在只能用于灰度图的直方图均衡化
void NonGeoOps::histogramEqualize(WImage& image){

//    int numTargetColor = 256;
    int outWidth = image.width(), outHeight = image.height();
    int total = outWidth * outHeight;
    std::vector<double> counts(256, 0.0);
    std::vector<int> colorTables(256,0);

    // count
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            counts[r] += 1;
        }
    }
    // count to ratio
    for(std::vector<double>::iterator it = counts.begin(); it != counts.end(); ++it){
        (*it) /= total;
    }
    // construct colorTable
    double c = 0.0;
    for(int i = 0; i < counts.size(); i ++){
        c += counts[i];
        colorTables[i] = int(c * 255);
    }

    // convert
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = image.pixel(i, j);
            int r = qRed(rgb);
            int tcolor = colorTables[r];
            image.setPixel(i, j, QColor(tcolor, tcolor, tcolor).rgb());
        }
    }

}

WImage NonGeoOps::histogramEqualize(const WImage& image){

}
