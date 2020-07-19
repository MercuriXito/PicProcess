#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMdiSubWindow>
#include <QFileDialog>
#include <QLabel>
#include <QDebug>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>
#include <QProgressDialog>

#include "wimage.h"
#include "widgets/qimgsubwindow.h"
#include "widgets/arithmdialog.h"
#include "lib/ops/geometryops.h"
#include "lib/ops/filterops.h"
#include "lib/ops/arithmops.h"
#include "lib/ops/basicops.h"
#include "lib/ops/nongeoops.h"
#include "lib/ops/fftops.h"
#include "lib/ops/kmeansclusterops.h"

void addsub(QWidget* wid);

QString getCurrentTime(){
    const QString& format = "hh_mm_ss_zzz";
    return QTime::currentTime().toString(format);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mdiAreaOriginal->setViewMode(QMdiArea::TabbedView);
    ui->mdiAreaOriginal->setTabsClosable(true);
    ui->listFiles->setExpandsOnDoubleClick(false);
    ui->listPFiles->setExpandsOnDoubleClick(false);

    ui->listFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listPFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // slots
    // open, close, save, display
    connect(ui->actionOpenFile, SIGNAL(triggered()), this, SLOT(openItems()));
    connect(ui->actionCloseFile, SIGNAL(triggered()), this, SLOT(closeSelectedItems()));
    connect(ui->actionCloseFile, SIGNAL(triggered()), this, SLOT(closeSelectedPItems()));
    connect(ui->actionSaveFile, SIGNAL(triggered()), this, SLOT(saveSelectedItems()));
    connect(ui->actionSaveFile, SIGNAL(triggered()), this, SLOT(saveSelectedPItems()));
    connect(ui->listFiles, SIGNAL(itemDoubleClicked(QTreeWidgetItem* , int)), this, SLOT(sendDisplayItem(QTreeWidgetItem* )));
    connect(ui->listPFiles, SIGNAL(itemDoubleClicked(QTreeWidgetItem* , int)), this, SLOT(sendDisplayPItem(QTreeWidgetItem* )));

    // process button
    connect(ui->actionMeanFilter, SIGNAL(triggered()), this, SLOT(callMeanFilter()));
    connect(ui->actionGaussianFilter, SIGNAL(triggered()), this, SLOT(callGaussianFilter()));
    connect(ui->actionMedianFilter, SIGNAL(triggered()), this, SLOT(callMedianFilter()));
    connect(ui->actionRobertsDetect, SIGNAL(triggered()), this, SLOT(callRobertsDetect()));
    connect(ui->actionSobelDetect, SIGNAL(triggered()), this, SLOT(callSobelDetect()));
    connect(ui->actionPrewittDetect, SIGNAL(triggered()), this, SLOT(callPrewittDetect()));
    connect(ui->actionLaplaceDetect, SIGNAL(triggered()), this, SLOT(callLaplaceDetect()));
    connect(ui->actionNegOps, SIGNAL(triggered()), this, SLOT(callNegOps()));
    connect(ui->actionToGrayScale, SIGNAL(triggered()), this, SLOT(callToGrayScale()));
    connect(ui->actionToBinary, SIGNAL(triggered()), this, SLOT(callToBinary()));
    connect(ui->actionScaleGrayExp, SIGNAL(triggered()), this, SLOT(callScaleGrayExp()));
    connect(ui->actionHistogramEqualize, SIGNAL(triggered()), this, SLOT(callHistogramEqualize()));
    connect(ui->actionFFTOps, SIGNAL(triggered()), this, SLOT(callFFTOps()));
    connect(ui->actionFFTAndIFFTOps, SIGNAL(triggered()), this, SLOT(callFFTAndIFFTOps()));
    connect(ui->actionMove, SIGNAL(triggered()), this, SLOT(callMove()));
    connect(ui->actionFlipHorizontal, SIGNAL(triggered()), this, SLOT(callFlipHorizontal()));
    connect(ui->actionFlipVertical, SIGNAL(triggered()), this, SLOT(callFlipVertical()));
    connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(callRotate()));
    connect(ui->actionScaleUp, SIGNAL(triggered()), this, SLOT(callScaleUp()));
    connect(ui->actionScaleDown, SIGNAL(triggered()), this, SLOT(callScaleDown()));
    connect(ui->actionKmeansClustering, SIGNAL(triggered()), this, SLOT(callKMeansClustering()));
    connect(ui->actionArithmOpses, SIGNAL(triggered()), this, SLOT(callArithmOps()));
    connect(ui->actionLaplaceSharpen, SIGNAL(triggered()), this, SLOT(callLaplaceSharpen()));

    // 有关Examples的响应
    connect(ui->actionDisplayRExample, SIGNAL(triggered()), this, SLOT(showRefactorExampleImage()));
    connect(ui->actionExampleRefactor, SIGNAL(triggered()), this, SLOT(callRefactor()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

// utils

QTreeWidgetItem* getQTreeWidgetItem(QString name){

    QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget* >(nullptr), QStringList(name));
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

    return item;
}

// 仅从目标 TreeWidget 移除 name 的 Item。
bool removeFromTreeWidget(QTreeWidget* widget, QString name){
    QList<QTreeWidgetItem *> findeditems = widget->findItems(name, Qt::MatchExactly, 0);
    qDebug() << findeditems.size();

    for(int i = 0 ; i < findeditems.size(); i ++){
        int idx = widget->indexOfTopLevelItem(findeditems[i]);
        delete widget->takeTopLevelItem(idx);
    }
}

// 仅从目标 TreeWidget 移除选中的 Item。
bool selectedRemoveFromTreeWidget(QTreeWidget* widget){
    QList<QTreeWidgetItem *> items = widget->selectedItems();
    qDebug() << items.size();

    for(int i = 0 ; i < items.size(); i ++){
        int idx = widget->indexOfTopLevelItem(items[i]);
        delete widget->takeTopLevelItem(idx);
    }
}

/* --------------------- Auxiliary Slots ---------------------------- */

bool MainWindow::sendDisplayItem(QTreeWidgetItem* item){
    item->setSelected(false);
    this->displayImage(item, true);
}

bool MainWindow::sendDisplayPItem(QTreeWidgetItem* item){
    item->setSelected(false);
    this->displayImage(item, false);
}

/* --------------------- Image Related Operation -------------------- */

WImage* MainWindow::searchImage(QString name, bool original){
    QHash<QString, WImage* >::iterator finded;
    if(original){
        finded = this->images.find(name);
    }else{
        finded = this->pimages.find(name);
    }
    if(finded == this->images.end()){
        return NULL;
    }else{
        return (*finded);
    }
}

QTreeWidget* MainWindow::searchTreeList(bool original){
    QTreeWidget* widget;
    if( original ){
        widget = ui->listFiles;
    }else{
        widget = ui->listPFiles;
    }
    return widget;
}

bool MainWindow::openImage(){
    QStringList files = QFileDialog::getOpenFileNames(
                this,  "选择打开的图像", ".", "Images (*.png *.jpg *.bmp)");
    for(QString file:files){
        this->openImage(file);
    }
}

bool MainWindow::openImage(QString filename){
    WImage* image = new WImage(filename, true);
    image->load(filename);
    qDebug() << image->format();
    this->images.insert(filename, image);

    // Update Ui
    ui->listFiles->addTopLevelItem(
                getQTreeWidgetItem(filename));
}

bool MainWindow::closeImage(QString name, bool original = true){

    QTreeWidget* widget = searchTreeList(original);
    QHash<QString, WImage*>* list;
    if( original ){
        list = &this->images;
    }else{
        list = &this->pimages;
    }

    QList<QTreeWidgetItem *> items = widget->findItems(name, Qt::MatchExactly);
    for(int i = 0; i < items.size(); i ++) {
        int idx = widget->indexOfTopLevelItem(items[i]);
        if(idx == -1){
            qDebug() << items[i]->text(0) << "Not found";
            return false;
        }
        delete widget->takeTopLevelItem(idx);
    }

    WImage* image = list->take(name);
    delete image;
    image = NULL;
}

bool MainWindow::closeImage(bool original = true){

    QTreeWidget* widget = this->searchTreeList(original);

    QList<QTreeWidgetItem *> selected = widget->selectedItems();
    qDebug() << selected.count() ;
    for(int i = 0; i < selected.size(); i ++) {
        QString name = selected[i]->text(0);
        int idx = widget->indexOfTopLevelItem(selected[i]);
        if(idx == -1){
            qDebug() << selected[i]->text(0) << "Not found";
            continue;
        }
        delete widget->takeTopLevelItem(idx);
        WImage* image;
        if(original){
            image = this->images.take(name);
        }else{
            image = this->pimages.take(name);
        }
        delete image;
        image = NULL;
    }
}

bool MainWindow::displayImage(WImage* image, bool repaint = false){
    if(image == NULL)
        return false;
    QMdiSubWindow* window = image->panel;
    if(window == NULL){
        QImgSubWindow* sub = new QImgSubWindow(image, ui->mdiAreaOriginal);
        image->panel = sub;
        window = sub;
    }else{
        if(repaint){
            delete window; // 重画
            QImgSubWindow* sub = new QImgSubWindow(image, ui->mdiAreaOriginal);
            image->panel = sub;
            window = sub;
        }else{
            ui->mdiAreaOriginal->setActiveSubWindow(window);
        }
    }
    window->show();
}

bool MainWindow::displayImage(QString name, bool original = true){
    WImage* images= this->searchImage(name, original);
    qDebug() << name;
    this->displayImage(images);
}

bool MainWindow::displayImage(QTreeWidgetItem *item, bool original = true){
    QString name = item->text(0);
    qDebug() << name;
    this->displayImage(name, original);
}

bool MainWindow::saveImage(QString name, bool original = true){
    QString saveFile = QFileDialog::getSaveFileName(this, QString("保存图像"), tr(""), tr("Images (*.png *.jpg *.bmp)"));
    if(saveFile.isNull()){
        return false;
    }
    WImage* image = this->searchImage(name, original);
    if(original){
        image->save(saveFile);
    }else{
        image->save(saveFile);
    }
}

/* ------------------- Items Related Operation ----------------------------- */

bool MainWindow::openItems(){
    this->openImage();
}

bool MainWindow::addItems(QString name, WImage* image){
    this->images.insert(name, image);
    ui->listFiles->addTopLevelItem(
                getQTreeWidgetItem(name));
}

bool MainWindow::addListItems(QList<QPair<QString, WImage* > > list){
    for(QPair<QString, WImage* > item: list){
        this->addItems(item.first, item.second);
    }
}

QPair<QString, WImage* > MainWindow::takeItem(QString name){
    WImage* image = this->images.take(name);
    removeFromTreeWidget(ui->listFiles, name);
    return QPair<QString, WImage* >(name, image);
}

QList<QPair<QString, WImage*> > MainWindow::takeSelectedItems(){
    QList<QPair<QString, WImage* > > list;
    QList<QTreeWidgetItem *> items = ui->listFiles->selectedItems();
    for(int i = 0 ; i < items.size(); i ++){
        QString name = items[i]->text(0);
        WImage* image = this->images.take(name);
        list.push_back(QPair<QString, WImage* >(name, image));
    }

    selectedRemoveFromTreeWidget(ui->listFiles);
    return list;
}

bool MainWindow::removeItems(QString name){
    removeFromTreeWidget(ui->listFiles, name);
}

bool MainWindow::removeSelectedItems(){
//    this->closeImage();
    QString name = ui->listFiles->topLevelItem(0)->text(0);
    qDebug() << name ;
    removeFromTreeWidget(ui->listFiles, name);
}

bool MainWindow::closeItems(QString str){
    this->closeImage(str, true);
}

bool MainWindow::closeSelectedItems(){
    this->closeImage(true);
}

bool MainWindow::saveItems(QString name){
   this->saveImage(name, true);
}

bool MainWindow::saveSelectedItems(){
    QList<QTreeWidgetItem *> items = ui->listFiles->selectedItems();
    for(int i = 0; i < items.size(); i ++){
        QString name = items[i]->text(0);
        this->saveImage(name, true);
    }
}

// Processed Images API
bool MainWindow::addPItems(QString name, WImage* image){
    this->pimages.insert(name, image);
    ui->listPFiles->addTopLevelItem(
                getQTreeWidgetItem(name));
}

QPair<QString, WImage*> MainWindow::takePItem(QString name){
    WImage* image = this->pimages.take(name);
    removeFromTreeWidget(ui->listPFiles, name);
    return QPair<QString, WImage* >(name, image);
}

QList<QPair<QString, WImage*> > MainWindow::takeSelectedPItems(){
    QList<QPair<QString, WImage* > > list;
    QList<QTreeWidgetItem *> items = ui->listPFiles->selectedItems();
    for(int i = 0 ; i < items.size(); i ++){
        QString name = items[i]->text(0);
        WImage* image = this->pimages.take(name);
        list.push_back(QPair<QString, WImage* >(name, image));
    }

    selectedRemoveFromTreeWidget(ui->listPFiles);
    return list;
}

bool MainWindow::removePItems(QString name){
    this->closeImage(name, false);
}

bool MainWindow::removeSelectedPItems(){
    this->closeImage(false);
}

bool MainWindow::movePItems(QString name){
    QPair<QString, WImage* > pitem = this->takePItem(name);
    WImage* image = pitem.second;
    image->setOpend(true);
    this->addItems(pitem.first, pitem.second);
}

bool MainWindow::moveSelectedPItems(){
    QList<QPair<QString, WImage *> > items = this->takeSelectedPItems();
    for(QList<QPair<QString, WImage *> >::iterator it = items.begin(); it != items.end(); it ++){
        (*it).second->setOpend(true);
    }
    this->addListItems(items);
}

bool MainWindow::closePItems(QString name){
    int button;
    button = QMessageBox::question(this, tr("中间结果保存"), tr("是否保存图像:") + name, QMessageBox::Yes | QMessageBox::No);
    if(button == QMessageBox::Yes){
        this->saveImage(name, false);
    }
    this->closeImage(name, false);
}

bool MainWindow::closeSelectedPItems(){
    QList<QTreeWidgetItem *> items = ui->listPFiles->selectedItems();
    for(int i = 0; i < items.size(); i ++){
        QString name = items[i]->text(0);
        this->closePItems(name);
    }
}

bool MainWindow::savePItems(QString name){
    bool ok = this->saveImage(name, false);
    if(ok){
        this->movePItems(name);
    }
}

bool MainWindow::saveSelectedPItems(){
    QList<QTreeWidgetItem *> items = ui->listPFiles->selectedItems();
    for(int i = 0; i < items.size(); i ++){
        QString name = items[i]->text(0);
        this->saveImage(name, false);
        this->movePItems(name);
    }
}

/* -------------------------- Outer lib response ----------------------- */
void MainWindow::respAddItem(QString name, WImage* image){
    if(image->isOpened()){
        this->addItems(name, image);
    }else{
        this->addPItems(name, image);
    }
}

void MainWindow::respChangeItem(QString name, WImage* image){
    if(!image->isOpened()) return; // 仅能 change非opened image
    WImage* target = this->searchImage(name, false);
    if(target == NULL) return ;
    this->closePItems(name);
    this->addPItems(name, image);
}

void MainWindow::respDisplayItem(WImage* image){
    this->displayImage(image);
}


void MainWindow::on_listFiles_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->listFiles->itemAt(pos);
    if(item == NULL) return;

    QMenu* menu = new QMenu(ui->listFiles);

    QAction* closeAction = new QAction("关闭图像", ui->listFiles);
    menu->addAction(closeAction);

}

/* -------------------------- Image Process Responded Operation ----------------------- */

// 获得 activate 窗口的图像，作为图像处理的输入
WImage* MainWindow::activatedImage(){

    QImgSubWindow* window = dynamic_cast<QImgSubWindow* >(ui->mdiAreaOriginal->activeSubWindow());
    if(window == NULL){
        QMessageBox::about(this, tr("提示") , tr("请先双击打开图像列表项选择待处理图像"));
        return NULL;
    }
    QString str = window->getImageName();
    bool opened = window->isOpened();

    WImage* selected;

    qDebug() << str << " " << opened ;

    if(opened ){ // 如果打开原图，那么拷贝一份用于处理，将拷贝添加到处理图像的列表中
        WImage* image = this->images[str];
        WImage* pimage = image->deepcopy();

        QString newName = image->getName() + QString("_process_") + getCurrentTime(); // TODO: 暂时命名成 Name + _process_ + 时间，避免重复
        pimage->setName(newName);
        pimage->setOpend(false);
        pimage->panel = NULL;

        this->addPItems(newName, pimage);
        selected = pimage;
    }else{ // 如果打开处理图像，继续处理
        selected = this->pimages[str];
    }

    return selected;
}

// 响应所有的图像处理信号
void MainWindow::callMeanFilter(){
    WImage* image = NULL;
    bool ok;
    int size = QInputDialog::getInt(this, tr("选择滤波大小"), tr("Pxiel"), 3, 3, 7, 2, &ok);
    if(ok){
        image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        QProgressDialog d(tr("进度"), NULL, 0, 100, this);
        d.setWindowModality(Qt::WindowModal);
        FilterOps::meanFiltered(*image, size, 0, &d);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callGaussianFilter(){
    WImage* image = NULL;
    bool ok;
    int size = QInputDialog::getInt(this, tr("选择滤波大小"), tr("Pxiel"), 3, 3, 7, 2, &ok);
    if(ok){
        image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        QProgressDialog d(tr("进度"), NULL, 0, 100, this);
        d.setWindowModality(Qt::WindowModal);
        FilterOps::gaussianFiltered(*image, size, 0, &d);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callMedianFilter(){
    WImage* image = NULL;
    bool ok;
    int size = QInputDialog::getInt(this, tr("选择滤波大小"), tr("Pxiel"), 3, 3, 7, 2, &ok);
    if(ok){
        image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        QProgressDialog d(tr("进度"), NULL, 0, 100, this);
        d.setWindowModality(Qt::WindowModal);
        FilterOps::medianFiltered(*image, size, 0, 1, &d);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callRobertsDetect(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FilterOps::robertsDetect(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callSobelDetect(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FilterOps::sobelDetect(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callPrewittDetect(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FilterOps::prewittDetect(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callLaplaceDetect(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FilterOps::laplaceDetect(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callNegOps(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    ArithmOps::neg(*image);
    this->displayImage(image, (!image->isOpened()));
}


void MainWindow::callToGrayScale(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    BasicOps::toGrayScale(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callToBinary(){
    bool ok;
    int threshold = QInputDialog::getInt(this, tr("选择二分的阈值"),
                                         tr("灰度值阈值"), 128, 0, 255, 1, &ok);
    if(ok){
        WImage* image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        BasicOps::toBinary(*image, threshold);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callScaleGrayExp(){
    bool ok;
    double exp = QInputDialog::getDouble(this, tr("选择变换指数"),
                                 tr("选择变换指数"), 1, 0.01, 20, 2, &ok);
    if(ok){
        WImage* image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        NonGeoOps::scaleGrayExp(*image, exp);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callHistogramEqualize(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    NonGeoOps::histogramEqualize(*image);
    this->displayImage(image, (!image->isOpened()));
}


void MainWindow::callFFTOps(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FFTOps::fft2dGray(*image);
    this->displayImage(image, (!image->isOpened()));
}


void MainWindow::callFFTAndIFFTOps(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FFTOps::fft2dThenIfft2d(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callArithmOps(){
    QString chosen1 = "", chosen2 = "";
    int ops = -1; uint flag = 0;
    if(this->images.size() == 0 && this->pimages.size() == 0){
        QMessageBox::about(this, tr("提示") , tr("没有可处理的图像，请先双击打开图像列表项选择待处理图像"));
        return ;
    }
    bool ok = ArithmDialog::doSelect(this,
                                     this->images.keys() + this->pimages.keys(),
                                     this->images.keys() + this->pimages.keys(),
                                     chosen1, chosen2, ops, flag);
    if(ok){
        qDebug() << " Start ! " << endl;

        WImage* w1 = this->searchImage(chosen1, true);
        WImage* w2 = this->searchImage(chosen2, true);
        if(w1 == NULL){
            w1 = this->searchImage(chosen1, false);
        }else{ // w1 是不是 opened 的图像还得处理下
            WImage* pimage = w1->deepcopy();
            QString newName = w1->getName() + QString("_process_") + getCurrentTime(); // TODO: 暂时命名成 Name + _process_ + 时间，避免重复
            pimage->setName(newName);
            pimage->setOpend(false);
            pimage->panel = NULL;
            this->addPItems(newName, pimage);
            w1 = pimage;
        }
        if(w2 == NULL){
            w2 = this->searchImage(chosen2, false);
        }

        // close Image 的时候没删除完全?

        switch( ops ){
        case 0: ArithmOps::add(*w1, *w2, flag) ;break;
        case 1: ArithmOps::minus(*w1, *w2, flag) ;break;
        case 2: ArithmOps::mul(*w1, *w2, flag);break;
        case 3: ArithmOps::div(*w1, *w2, flag);break;
        case 4: ArithmOps::neg(*w1);break;
        case 5: ArithmOps::andI(*w1, *w2, flag);break;
        case 6: ArithmOps::orI(*w1, *w2, flag);break;
        }

        this->displayImage(w1,!(w1->isOpened()));
    }

}

void MainWindow::callMove(){
    bool ok1, ok2;

    WImage* image = this->activatedImage();
    if(image == NULL) return;
    int moveX = QInputDialog::getInt(this, tr("选择平移"),
                                     tr("选择水平平移距离"),
                                     0, - image->width(), image->width(), 1, &ok1);
    int moveY = QInputDialog::getInt(this, tr("选择平移"),
                                     tr("选择垂直平移距离"),
                                     0, - image->height(), image->height(), 1, &ok2);
    if(ok1 & ok2){
        if(image == NULL){
            return ;
        }
        GeometryOps::move(*image, moveX, moveY);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callFlipHorizontal(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    GeometryOps::flip(*image, true);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callFlipVertical(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    GeometryOps::flip(*image, false);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::callRotate(){
    bool ok;
    double angle = QInputDialog::getDouble(this, tr("选择角度"),
                                 tr("选择顺时针旋转角度"), 90, 0.0, 360, 2, &ok);
    if(ok){
        WImage* image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        GeometryOps::rotate(*image, angle);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callScaleUp(){
    bool ok;
    double ratio = QInputDialog::getDouble(this, tr("选择比例"),
                                 tr("选择放大比例"), 1.0, 1.0, 5.0, 2, &ok);
    if(ok){
        WImage* image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        GeometryOps::scaleUp(*image, ratio);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callScaleDown(){
    bool ok;
    double ratio = QInputDialog::getDouble(this, tr("选择比例"),
                                 tr("选择缩小比例"), 1.0, 1.0, 5.0, 2, &ok);
    if(ok){
        WImage* image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        GeometryOps::scaleDown(*image, 1 / ratio);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callRefactor(){
    bool ok;
    int size = QInputDialog::getInt(this, tr("选择重构的项数"),
                                    tr("目标重构项数"),
                                    this->d.getNumItem() / 2, 1, this->d.getNumItem(), 1, &ok);
    if(ok){
        this->d.refactorItem(size);
        this->showRefactoredExampleImage();
    }
}

void MainWindow::showRefactorExampleImage(){
    QImage image = this->d.show(true);
    QString name("RefactorExample" + getCurrentTime());
    WImage* img = new WImage(image);
    img->setName(name);
    img->setOpend(false);
    this->addPItems(name, img);
    this->displayImage(img, true);
}

void MainWindow::showRefactoredExampleImage(){
    QImage image = this->d.show(false);
    QString name("RefactorExample" + getCurrentTime());
    WImage* img = new WImage(image);
    img->setName(name);
    img->setOpend(false);
    this->addPItems(name, img);
    this->displayImage(img, true);
}

void MainWindow::callKMeansClustering(){
    WImage* image = NULL;
    bool ok;
    int k = QInputDialog::getInt(this, tr("选择聚类中心个数"),
                                 tr("K"), 2, 2, 10, 1, &ok);
    if(ok){
        image = this->activatedImage();
        if(image == NULL){
            return ;
        }
        QProgressDialog d(tr("聚类进度：迭代轮数和最大迭代数比例"), NULL, 0, 100, this);
        d.setWindowModality(Qt::WindowModal);
        KMeansClusterOps ops;
        ops.cluster(*image, k, 500, 1e-2, &d);
        this->displayImage(image, (!image->isOpened()));
    }
}

void MainWindow::callLaplaceSharpen(){
    WImage* image = this->activatedImage();
    if(image == NULL){
        return ;
    }
    FilterOps::laplaceSharpen(*image);
    this->displayImage(image, (!image->isOpened()));
}

void MainWindow::on_clickDown_clicked()
{
    QList<QTreeWidgetItem *> items = ui->listFiles->selectedItems();
    for(int i = 0; i < items.size(); i ++){
        QString str = items[i]->text(0);

        WImage* image = this->images[str];
        WImage* pimage = image->deepcopy();

        QString newName = image->getName() + QString("_process_") + getCurrentTime(); // TODO: 暂时命名成 Name + _process_ + 时间，避免重复
        pimage->setName(newName);
        pimage->setOpend(false);
        pimage->panel = NULL;

        this->addPItems(newName, pimage);
    }
}

void MainWindow::on_clickUp_clicked()
{
    this->moveSelectedPItems();
}
