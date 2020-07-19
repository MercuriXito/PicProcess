#include "kmeansclusterops.h"

#include <QDebug>
#include "wimage.h"
#include <QProgressDialog>

KMeansClusterOps::KMeansClusterOps()
{

}

inline double l2Distance(const Vector& v1, const Vector& v2){
    double sum = 0;
    for(size_t i = 0; i < v1.size(); ++i){
        sum += pow((v1[i] - v2[i]), 2);
    }
    return sqrt(sum);
}

inline double l2(const Vector& v){
    double sum = 0;
    for(Vector::const_iterator it = v.begin(); it != v.end(); it++){
        sum += pow((*it), 2);
    }
    return sqrt(sum);
}

inline void subto(Vector& v1, const Vector& v2){
    for(size_t i = 0; i < v1.size(); ++i){
        v1[i] -= v2[i];
    }
}

inline void addto(Vector& v1, const Vector& v2){
    for(size_t i = 0; i < v1.size(); ++i){
        v1[i] += v2[i];
    }
}

inline Vector rgb2vec(const QRgb& q){
    Vector vec(3, 0);
    vec[0] = qRed(q);
    vec[1] = qGreen(q);
    vec[2] = qBlue(q);
    return vec;
}

inline QRgb vec2rgb(const Vector& vec){
    return QColor(int(std::round(vec[0])), int(std::round(vec[1])), int(std::round(vec[2]))).rgb();
}

inline QString vec2str(const Vector& vec){
    QString s("(");
    for(Vector::const_iterator it = vec.begin(); it != vec.end(); it++){
        s += QString::number(*it);
        s += (",");
    }
    s += (")");
    return s;
}

inline QString dmat2str(const DMat& mat){
    QString s("");
    for(DMat::const_iterator it = mat.begin(); it != mat.end(); it ++ ){
        s += vec2str(*it); s += " ";
    }
    return s;
}

DMat KMeansClusterOps::getAverageCenter(int k, int numFeature, double inf, double sup){
    DMat centroids(k, Vector(numFeature, 0));
    for(int i = 0; i < k; ++i){
        for(int j = 0; j < numFeature; ++j){
            centroids[i][j] = inf + (sup - inf) / (k - 1) * i;
        }
    }
    return centroids;
}

DMat KMeansClusterOps::getRandonCenter(int k, int numFeature, double inf, double sup){
    // TODO:
}

void KMeansClusterOps::cluster(WImage &image, int k, int maxIter, double threshold, QProgressDialog* d){

    int w = image.width(), h = image.height();
    int num = w * h;
    int numFeature = 3;

    DMat centroid = this->getAverageCenter(k, numFeature, 0, 255);

    int iter = 0;
    double lastErr = 0xffffffff, thisErr = 0;
    double err = lastErr - thisErr;

    IdxVector belongs(num, 0); // 记录各像素点的当前聚类的类
//    Vector minLen(num, 0x0fffffff); // 距离各像素点到其所属的聚类中心的距离

    // O(iter * w * h * k * numFeature) ... 复杂度也太高了 ...
    qDebug() << "Start KMeans";
    while(iter < maxIter){
        if(iter > 1 && err < threshold) break;
        ++ iter;

        qDebug() << dmat2str(centroid);

        double sumErr = 0; // 该轮总 err
        IdxVector counts(k, 0); // 该轮各类包含的点数
        DMat centers(k, Vector(3, 0)); // 包含点

        // 遍历所有样本，根据到各聚类中心的距离分配每个点的中心
        for(int i = 0; i < w; ++i){
            for(int j = 0; j < h; ++j){
                double lesslen = 0x0fffffff; int center = 0;
                Vector feature = rgb2vec(image.pixel(i,j));
                for(int a = 0; a < k; ++a){
                    int len = l2Distance(feature, centroid[a]);
                    if(lesslen > len){
                        lesslen = len; center = a;
                    }
                }
                belongs[i * h + j] = center;
                addto(centers[center], feature);
                sumErr += lesslen;
                counts[center] ++;
            }
        }

        sumErr /= float(num);
        lastErr = thisErr;
        thisErr = sumErr;
        err = std::abs(lastErr - thisErr);
        qDebug() << iter << ": " << lastErr << " , " << thisErr;

        // 调整聚类中心
        for(int a = 0; a < k; ++a){
            for(int i = 0; i < numFeature; ++i){
                centroid[a][i] = centers[a][i] / counts[a];
            }
        }
        if(d){
            d->setValue(int(iter / maxIter * 100));
        }
    }

    // 根据聚类结果分配效果咯
    for(int i = 0; i < w; ++i){
        for(int j = 0; j < h; ++j){
            Vector vec = centroid[belongs[i * h + j]];
            image.setPixel(i, j, vec2rgb(vec));
        }
    }
}

WImage KMeansClusterOps::cluster(const WImage &image, int k, int maxIter, double threshold){

}
