#ifndef NONGEOOPS_H
#define NONGEOOPS_H

#include "lib/ops/ops.h"

class WImage;

class NonGeoOps: public Ops
{
public:
    NonGeoOps();

    static double texp;
    inline static void expScale(int r, int g, int b, int& tr, int& tg, int& tb);

    // 直方图均衡化
    static void histogramEqualize(WImage& image);
    static WImage histogramEqualize(const WImage& image);

    // 灰度级变化
    static void scaleGrayExp(WImage& image, double exp);
    static WImage scaleGrayExp(const WImage& image, double exp);


};

#endif // NONGEOOPS_H
