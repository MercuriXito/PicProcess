#include "discriptorexample.h"

#include <QDebug>

#include "lib/ops/fftops.h"
#include "wimage.h"

bool sortPair(std::pair<int, int>& p1, std::pair<int, int>& p2){
    if(p1.first < p2.first) return true;
    else if(p1.first > p2.first) return false;
    if(p1.first == p2.first) return p1.second < p2.second;
}

void sortAxes(std::vector<std::pair<int, int> >& t){
    std::sort(t.begin(), t.end(), sortPair);
}

DiscriptorExample::DiscriptorExample()
{
    // 默认的例子，N = 64 的正方形边界
    this->boundary = std::vector<std::pair<int, int> >(64);

    int x = 20, y = 20, count = 0;
//    for(int i = 0; i <= 16; i ++, count ++ ){
//        this->boundary[count] = std::pair<int, int>(x, y + 10 * i);
//    }
//    for(int i = 1; i < 16; i ++){
//        this->boundary[count] = std::pair<int, int>(x + 10 * i, y); count ++;
//        this->boundary[count] = std::pair<int, int>(x + 10 * i, y + 10 * 16); count ++;
//    }
//    for(int i = 0; i <= 16; i ++, count ++ ){
//        this->boundary[count] = std::pair<int, int>(x + 10 * 16, y + 10 * i);
//    }

    // 注意一定是逆时针顺序
    for(int i = 0; i < 16; i ++, count ++ ){
        this->boundary[count] = std::pair<int, int>(x + 10 * i, y);
    }
    for(int i = 0; i < 16; i ++, count ++ ){
        this->boundary[count] = std::pair<int, int>(x + 10 * 16, y + 10 * i);
    }
    for(int i = 0; i < 16; i ++, count ++ ){
        this->boundary[count] = std::pair<int, int>(x + 10 * (16 - i), y + 10 * 16);
    }
    for(int i = 0; i < 16; i ++, count ++ ){
        this->boundary[count] = std::pair<int, int>(x , y + 10 * (16 - i));
    }
    this->init();
}

void DiscriptorExample::init(){
    this->refactored = this->boundary;
    this->width = 200;
    this->height = 200;
//    sortAxes(this->boundary);
}

DiscriptorExample::DiscriptorExample(std::vector<std::pair<int, int> > b){
    this->boundary = b;
    this->init();
}

void DiscriptorExample::refactorItem(int m){
    if(size_t(m) > this->boundary.size()) return;
    int size = this->boundary.size();
    Complex* res = new Complex[size];
    for(int i = 0; i < size; i ++ ){
//        qDebug() << this->boundary[i].first << " " << this->boundary[i].second;
        res[i] = Complex(this->boundary[i].first, this->boundary[i].second);
    }
    FFTOps::dft1d(res, size, false);
    FFTOps::dft1d(res, size, true, m);
    this->refactored.clear();
    for(int i = 0; i < size; i ++ ){
        Complex t = res[i];
//        qDebug() << t.real() << " " << t.imag();
        this->refactored.push_back(std::make_pair<int, int>( int(round(t.real())), int(round(t.imag()) ) ));
    }
}

QImage DiscriptorExample::show(bool original){
    QImage image(this->width, this->height, QImage::Format_RGB32);
    for(int i = 0; i < image.width(); ++i){
        for(int j = 0; j < image.height(); ++j){
            image.setPixel(i,j,0);
        }
    }
    std::vector<std::pair<int, int> >* target;
    int x,y ;
    if( original ){
        target = &this->boundary;
    }else{
        target = &this->refactored;
    }
    for(std::vector<std::pair<int, int> >::iterator it = target->begin(); it != target->end(); it ++ ){
        x = (*it).first; y = (*it).second;
        image.setPixel(x, y, QColor(255, 255, 255).rgb());
    }
    return image;
}

int DiscriptorExample::getNumItem() const{
    return this->boundary.size();
}



