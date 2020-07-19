#ifndef KMEANSCLUSTEROPS_H
#define KMEANSCLUSTEROPS_H

#include <vector>

// 勉强用 vector 当作 Vector 和 Mat:
typedef std::vector<double> Feature;
typedef std::vector<double> Vector;
typedef std::vector<int> IdxVector;
typedef std::vector<std::vector<double> > DMat;
class WImage;
class QProgressDialog;

class KMeansClusterOps
{
public:
    KMeansClusterOps();

    // 初始化聚类中心方法
    DMat getAverageCenter(int k, int numFeature, double inf, double sup);
    DMat getRandonCenter(int k, int numFeature, double inf, double sup);

    //
    void cluster(WImage& image, int k, int maxIter = 500, double threshold = 1e-2, QProgressDialog* d = 0);
    WImage cluster(const WImage& image, int k, int maxIter = 500, double threshold = 1e-2);

};
#endif // KMEANSCLUSTEROPS_H
