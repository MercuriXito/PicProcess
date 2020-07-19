#ifndef GEOMETRYOPS_H
#define GEOMETRYOPS_H

class WImage;

// 变换矩阵
//typedef struct matrix{
//    double* mat; // 固定3x3
//    int w,h;
//    matrix(){mat = new double[3 *3];}
//    matrix(double* mat){}
//    matrix(const struct matrix& m){}
//    struct matrix operator=(const struct matrix& mat){}
//}TransMatrix;

class GeometryOps
{
public:
    GeometryOps();

//    static void transform(WImage& image, const TransMatrix& mat);

    // 镜像
    static void flip(WImage& image, bool horizon);
    static WImage flip(const WImage& image, bool horizon);

    // 旋转
    static void rotate(WImage& image, float angle);
    static WImage rotate(const WImage& image, float angle);

    // 平移
    static void move(WImage& image, int x, int y);
    static WImage move(const WImage& image, int x, int y);

    // 缩放
    static void scale(WImage& image, double ratio);
    static WImage scale(const WImage& image, double ratio);

    // 放大
    static void scaleUp(WImage& image, double ratio);
    static WImage scaleUp(const WImage& image, double ratio);

    // 缩小
    static void scaleDown(WImage& image, double ratio);
    static WImage scaleDown(const WImage& image, double ratio);

    // 拉伸
};

#endif // GEOMETRYOPS_H
