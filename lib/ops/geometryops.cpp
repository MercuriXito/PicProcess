#include "geometryops.h"

#include "wimage.h"

GeometryOps::GeometryOps()
{

}

const float pi = 3.1415926;

inline QRgb Pixel(const QImage& image, int x, int y, int padding = 0){
    if(x < 0 || x >= image.width() || y < 0 || y >= image.height() ) return QColor(padding, padding, padding).rgb();
    return image.pixel(x, y);
}

//void GeometryOps::transform(WImage& image, const TransMatrix& mat){

//}

// 镜像
void GeometryOps::flip(WImage& image, bool horizon){
    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb;
            if(horizon)  rgb = temp->pixel(outWidth -  i, j);
            else   rgb = temp->pixel(i, outHeight - j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            image.setPixel(i, j, QColor(r, g, b).rgb());
        }
    }
    delete temp;
}

WImage GeometryOps::flip(const WImage& image, bool horizon){
    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb;
            if(horizon)  rgb = image.pixel(outWidth -  i, j);
            else   rgb = image.pixel(i, outHeight - j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            temp->setPixel(i, j, QColor(r, g, b).rgb());
        }
    }
    return *temp;
}

// 旋转
void GeometryOps::rotate(WImage& image, float angle){

    // TODO: 目前旋转是使用替换的方法实现，由于数值问题，存在失真现象

    // 现在是绕着(0,0)旋转，怎么变成绕中心旋转呢？
    double hw = image.width() / 2.0;
    double hh = image.height() / 2.0;

    float cha = angle - int(angle);
    angle = (int(angle) % 360) + cha;
    float piangle = angle / 180 * pi;

    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){ // 绕中心旋转相当于平移->旋转->平移
            double xx = i - hw, yy = j - hh;
            double orx = xx * cos(piangle) + yy * sin(piangle);
            double ory = - xx * sin(piangle) + yy * cos(piangle);
            image.setPixel(i, j, Pixel(*temp, orx + hw, ory + hh));
        }
    }

    delete temp;
}

WImage GeometryOps::rotate(const WImage& image, float angle){

    float cha = angle - int(angle);
    angle = (int(angle) % 360) + cha;
    float piangle = angle / 180 * pi;

    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            int orx = int(i * cos(piangle) + j * sin(piangle));
            int ory = int(- i * sin(piangle) + j * cos(piangle));
            temp->setPixel(i, j, Pixel(image, orx, ory));
        }
    }

    return *temp;
}

// 平移
void GeometryOps::move(WImage& image, int x, int y){
    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = Pixel(*temp, i - x, j - y, 0);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            image.setPixel(i, j, QColor(r, g, b).rgb());
        }
    }
    delete temp;
}

WImage GeometryOps::move(const WImage& image, int x, int y){
    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            QRgb rgb = Pixel(image, i - x, j - y, 0);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            temp->setPixel(i, j, QColor(r, g, b).rgb());
        }
    }
    return *temp;
}

// 缩放
void GeometryOps::scale(WImage& image, double ratio){
    if(ratio > 1) GeometryOps::scaleUp(image, ratio);
    else if(ratio < 1 ) GeometryOps::scaleDown(image, ratio);
}

WImage GeometryOps::scale(const WImage& image, double ratio){
    WImage* res = image.deepcopy();
    GeometryOps::scale(*res, ratio);
    return *res;
}

// 中心放大，需要插值, ratio > 1
// ratio = new_width / width, 同时保持比例不变
void GeometryOps::scaleUp(WImage& image, double ratio){

    int nw = int(image.width() / ratio);
    int nh = int(image.height() / ratio);

    int ws = ( image.width() - nw ) / 2;
    int hs = ( image.height() - nh ) / 2;

    QImage temp = image.copy(ws, hs, nw, nh);
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            int orx = int( i / ratio );
            int ory = int( j / ratio );
            image.setPixel(i, j, temp.pixel(orx, ory));
        }
    }
}

WImage GeometryOps::scaleUp(const WImage& image, double ratio){
    int nw = int(image.width() / ratio);
    int nh = int(image.height() / ratio);

    int ws = ( image.width() - nw ) / 2;
    int hs = ( image.height() - nh ) / 2;

    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            int orx = int( i / ratio );
            int ory = int( j / ratio );
            temp->setPixel(i, j, image.pixel(orx, ory));
        }
    }

    return *temp;
}

// 中心缩小，对应映射就行, ratio < 1
// ratio = new_width / width, 同时保持比例不变
void GeometryOps::scaleDown(WImage& image, double ratio){

    int nw = int(image.width() * ratio);
    int nh = int(image.height() * ratio);

    int ws = ( image.width() - nw ) / 2;
    int hs = ( image.height() - nh ) / 2;
    int we = ws + nw;
    int he = hs + nh;

    WImage* temp = image.deepcopy();

    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            if(i < ws || i >= we || j < hs || j >= he){
                image.setPixel(i, j, 0);
                continue;
            }
            int orx = int( (i - ws) / ratio);
            int ory = int( (j - hs) / ratio);
            image.setPixel(i,j,temp->pixel(orx, ory));
        }
    }

    delete temp;
}

WImage GeometryOps::scaleDown(const WImage& image, double ratio){
    int nw = int(image.width() * ratio);
    int nh = int(image.height() * ratio);

    int ws = ( image.width() - nw ) / 2;
    int hs = ( image.height() - nh ) / 2;
    int we = ws + nw;
    int he = hs + nh;

    WImage* temp = image.deepcopy();
    int outWidth = image.width();
    int outHeight = image.height();
    for(int i = 0; i < outWidth; ++i){
        for(int j = 0; j < outHeight; ++j){
            if(i < ws || i >= we || j < hs || j >= he){
                temp->setPixel(i, j, 0);
                continue;
            }
            int orx = int( (i - ws) / ratio);
            int ory = int( (j - hs) / ratio);
            temp->setPixel(i,j,image.pixel(orx, ory));
        }
    }

    return *temp;
}
