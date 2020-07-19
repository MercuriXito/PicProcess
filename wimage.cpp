#include "wimage.h"

#include "widgets/qimgsubwindow.h"

#include <QDebug>

#define debug( x ) qDebug() << x ;

inline bool checkBinary(const WImage& image){
    bool s = true;
    for(int i  = 0; i < image.width(); i ++){
        for(int j = 0; j < image.height(); j++){
            QRgb rgb = image.pixel(i,j);
            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);
            s &= ((r == 0 || r == 255) && (g == 0 || g == 255) && (b == 0 || b == 255) );
        }
    }
    return s;
}

void WImage::init(QString name = "", bool opened = false, QImgSubWindow* panel = NULL,
                  int isGray = -1, int isBinary = -1){

    this->opened = opened;
    this->panel = panel;
    this->name = name;
    this->gray = isGray;
    this->binary = isBinary;
}

WImage::WImage(bool opened):QImage()
{
    init("", opened);
}

WImage::WImage(QString name, bool opened):QImage()
{
    init(name, opened);
}

// constructor with shallow copy of QImage Object
WImage::WImage(QString name, bool opened, const QImage& image):QImage(image){
    init(name, opened, NULL);
}

WImage::WImage(const QImage& image):QImage(image){
    init();
}

WImage::WImage(const WImage& image):QImage(image){
    init(image.name, image.opened);
}

WImage::WImage(int width, int height, Format f):QImage(width, height, f){
    init();
}

WImage::~WImage(){
    if(this->panel != NULL){
        this->panel->close();
        delete this->panel;
        this->panel = NULL;
    }
}

const QString& WImage::getName() const{
    return this->name;
}

bool WImage::isOpened() const{
    return this->opened;
}

void WImage::setName(QString str){
    this->name = str;
}

void WImage::setOpend(bool opened){
    this->opened = opened;
}


int WImage::isGrayscale(){
    if(this->gray == -1){
        this->gray = int(QImage::isGrayscale());
    }
    return this->gray;
}

int WImage::isGrayscale() const{
    if(this->gray == -1){
        return QImage::isGrayscale();
    }
    return this->gray;
}

int WImage::isBinary(){
    if(this->binary == -1){
        this->binary = checkBinary(*this);
    }
    return this->binary;
}

int WImage::isBinary() const{
    if(this->binary == -1){
        return checkBinary(*this);
    }
    return this->binary;
}

WImage* WImage::deepcopy() const{
    debug(tr("WImage deepcopied: ") + this->name);
    const QImage& copied =  QImage::copy();
    WImage* image = new WImage(this->getName(), this->isOpened(), copied);
    return image;
}

//bool WImage::convertToFormat(Format format, Qt::ImageConversionFlag flags){
//    if(format == QImage::Format_Grayscale8){
////		QImage image = QImage::convertToFormat(format, flags);
////		for(int i = 0; i < this->width(); i ++){
////			for(int j = 0; j < this->height(); j ++){
////				this->setPixel(i, j, image.pixel(i,j));
////			}
////		}
////		this->reinterpretAsFormat(format);
////	//    this->convertToFormat_inplace(format, flags);
//        for(int i = 0; i < this->width(); i ++){
//            for(int j = 0; j < this->height(); j ++){
//                QRgb rgb = this->pixel(i, j);
//                int r = qRed(rgb);
//                int g = qGreen(rgb);
//                int b = qBlue(rgb);
//                int avg = (r + g + b) / 3;
//                this->setPixel(i, j, qRgb(avg, avg, avg) );
//            }
//        }
//    }
//}

bool WImage::operator=(const WImage & image){
    this->loadFromData(image.bits(), image.byteCount()); // TODO: 总感觉这里有问题呢
    this->setName(image.name);
    this->setOpend(image.opened);
}
