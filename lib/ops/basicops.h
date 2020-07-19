#ifndef BASICOPS_H
#define BASICOPS_H

#include <cassert>
#include "lib/ops/ops.h"

class WImage;

typedef unsigned char uchar;

typedef struct hsipixel{
    double i; // i \in [0,1]
    double s; // s \in [0,1]
    double h; // h \in [0,360]
    hsipixel():h(0),s(0),i(0){};
    hsipixel(double h, double s, double i):h(h),s(s),i(i){};
    hsipixel(const struct hsipixel& p){this->h = p.h; this->s = p.s; this->i = p.i;};
    bool operator=(const struct hsipixel& p){this->h = p.h; this->s = p.s; this->i = p.i;};
}HsiPixel;

class HSIImage{
public:
    HsiPixel* ptr;
    int h, w;

    HSIImage(){};
    explicit HSIImage(int h, int w, bool allocate = false):h(h), w(w){ if(allocate) this->ptr = new HsiPixel[h * w]; else this->ptr = NULL;};
    HSIImage(const HSIImage& image){ // shallow copy
        this->h = image.h; this->w = image.w;
        this->ptr = image.ptr;
    };
    HsiPixel operator()(int i, int j) const{
        assert(( (i > 0) && (i < this->w) ) && ((j > 0)&&(j < this->h)));
        return this->ptr[i * this->w + j];
    };
    int width() const{ return this->w;};
    int height() const {return  this->h;};
};

class BasicOps : public Ops
{
public:
    BasicOps();

    static void toGrayScale(WImage& image);
    static WImage toGrayScale(const WImage& image);

    static void toBinary(WImage& image, int threshold);
    static WImage toBinary(const WImage& image, int threshold);

    static HSIImage rgb2hsv(const WImage& image);
    static WImage hsv2rgb(const HSIImage& image);
};

#endif // BASICOPS_H
