#ifndef FILTEROPS_H
#define FILTEROPS_H

#include <iostream>
#include <vector>
#include "lib/ops/ops.h"

class WImage;
class QProgressDialog;

typedef struct kernel{
    std::vector<double>* mat;
    int height, width;
    kernel(std::vector<double>* mat, int width, int height):
        height(height),width(width),mat(mat){};
    kernel(double arr[], int width, int height):height(height), width(width) {
        this->mat = new std::vector<double>(); int i = 0;
        while(i < height * width){ this->mat->push_back(arr[i]); ++i;}
    }
    kernel(int width, int height):height(height), width(width){
        this->mat = new std::vector<double>(height * width, 0 );}
    kernel(){};
    double operator()(int x, int y) const{return this->mat->at(x * this->width + y);}
}Kernel;

class FilterOps: public Ops
{
public:
    FilterOps();

    typedef enum PaddingMode{
        PADDING_NONE,
        PADDING_ZERO,
        PADDING_COPY,
        PADDING_REVERSE,
    }PaddingMode;

public:
    // Linear Filter
    static bool filter(const Kernel*, WImage&, int stride = 1, PaddingMode mode = PADDING_ZERO, QProgressDialog* bar = NULL);
    static const WImage& filter2(const Kernel*, const WImage&, int stride = 1, PaddingMode mode = PADDING_ZERO);
    static bool filterF(const Kernel*, WImage*, double (* cal)(const Kernel*, WImage* ), int stride = 1, PaddingMode = PADDING_ZERO);

    static  Kernel* gaussianKernel(int width, int height = 0);
    static  Kernel* meanKernel(int width, int height = 0);
    static  std::vector<Kernel* >* sobelKernel();
    static  Kernel* lapLaceKernel();
    static  std::vector<Kernel* >* prewittKernel();
    static  std::vector<Kernel* >* robertsKernel();

    //
    static bool meanFiltered(WImage& , int , int  = 0, QProgressDialog* bar = NULL);
    static bool gaussianFiltered(WImage& , int , int = 0,QProgressDialog* bar = NULL);
    static bool medianFiltered(WImage& , int , int = 0, int = 1, QProgressDialog* bar = NULL);

    static bool laplaceDetect(WImage& );
    static bool robertsDetect(WImage& );
    static bool sobelDetect(WImage& );
    static bool prewittDetect(WImage& );

    static bool laplaceSharpen(WImage& );

};

#endif // FILTEROPS_H
