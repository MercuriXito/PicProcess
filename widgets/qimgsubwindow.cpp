#include "qimgsubwindow.h"
#include "ui_qimgsubwindow.h"

#include "wimage.h"

QImgSubWindow::QImgSubWindow(WImage* image, QWidget *parent) :
    QMdiSubWindow(parent),
    ui(new Ui::QImgSubWindow),
    image(image)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->init();
    this->draw();
}

QImgSubWindow::~QImgSubWindow()
{
    image->panel = NULL;
    delete ui;
}

void QImgSubWindow::closeEvent(QCloseEvent* event){
    emit sendClose(image);
    QMdiSubWindow::closeEvent(event);
}

void QImgSubWindow::init(){

//    this->autoFillBackground();
//    QPixmap pixmap("resources/back.jpg");
//    QPalette palette;
//    palette.setBrush(QPalette::Window, QBrush(pixmap));
//    this->setPalette(palette);

    // scale the image
    this->iw = image->width();
    this->ih = image->height();

    QRect window = this->geometry();
    int ww = window.width();
    int wh = window.height();

    float windowRatio = ww / float(wh);
    float imageRatio = this->iw / float(this->ih);

    if(windowRatio < imageRatio ){ // 按宽放缩
        this->iw = ww;
        this->ih = int(ww / imageRatio);
        this->x = 0;
        this->y = int( ( wh - this->ih )/ 2 );
    }else{ // 按高放缩
        this->ih = wh;
        this->iw = int(wh * imageRatio );
        this->x = int( (ww - this->iw) / 2 );
        this->y = 0;
    }

    this->w = this->iw;
    this->h = this->ih;

//    qDebug() << image->width() << " " << image->height() << " " << windowRatio << " " << imageRatio << "" << ww << " " << wh << endl;
//    qDebug() << this->iw << " " << this->ih << " " << this->x << " " << this->y << " " << endl;

}

void QImgSubWindow::draw(){
    QImage cpimg = this->image->copy();
    cpimg = cpimg.scaled(this->w, this->h, Qt::KeepAspectRatio);
    ui->imageLabel->setScaledContents(true);
    ui->imageLabel->setGeometry(x, y, w, h);
    ui->imageLabel->setPixmap(QPixmap::fromImage(cpimg));
}

const QString QImgSubWindow::getImageName(){
    return this->image->getName();
}

const bool QImgSubWindow::isOpened(){
    return this->image->isOpened();
}
