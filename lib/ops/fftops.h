#ifndef FFTOPS_H
#define FFTOPS_H

#include <complex>
#include "lib/ops/ops.h"

class WImage;

typedef unsigned char uchar;
typedef std::complex<float> Complex;

class FFTOps : public Ops
{
public:
    FFTOps();

    // 基础的 FFT1d 实现
    static void dft1d(Complex* a, int len, bool inverse = false, int truncation = -1);
    static void fft1dRecur(Complex* a , int len);
    static void fft1dBf(Complex* a, int len, bool inverse);
    static void ifft1dBf(Complex* a, int len);

    static void fft2d(Complex* pic, int outWidth, int outHeight);
    static void ifft2d(Complex* pic, int outWidth, int outHeight);

    static void fft2dGray(WImage& image, bool doCenterShift = true, bool doCompress = true);
    static void ifft2dGray(WImage& image, Complex* pic, bool doCenterShift = true, bool doCompress = true);
    static void fft2dThenIfft2d(WImage& image);
    static void fft2d(WImage& image);
    static WImage fft2d(const WImage& image);
};

#endif // FFTOPS_H
