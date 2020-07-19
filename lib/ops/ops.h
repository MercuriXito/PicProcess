#ifndef OPS_H
#define OPS_H

class WImage;

class Ops
{
public:
    Ops();
    static void pixelModify(WImage& image, unsigned int (*func)(int, int, int));
    static void pixelModify(WImage& image, void (*func)(int, int, int, int&, int&, int&)); 

};

#endif // OPS_H
