#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wimage.h"
#include "lib/discriptorexample.h"

class QTreeWidget;
class QTreeWidgetItem;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QHash<QString, WImage*> images;
    QHash<QString, WImage*> pimages;

public slots:
    // 辅助信号
    bool sendDisplayItem(QTreeWidgetItem* );
    bool sendDisplayPItem(QTreeWidgetItem* );

private:
    WImage* searchImage(QString , bool);
    QTreeWidget* searchTreeList(bool);

public: // Image Related Operation
    bool openImage();
    bool openImage(QString  );
    bool closeImage(QString, bool);
    bool closeImage(bool);
    bool saveImage(QString , bool);
    bool displayImage(QString , bool );
    bool displayImage(QTreeWidgetItem*, bool);
    bool displayImage(WImage* , bool repaint);

private slots:

    // take 仅在列表、显示列表中删除，但是不删除 WImage*， remove 在 take 之上删除 WImage*
    bool openItems();
    bool addItems(QString , WImage* );
    bool addListItems(QList<QPair<QString, WImage* > > );
    QPair<QString, WImage*> takeItem(QString );
    QList<QPair<QString, WImage*> > takeSelectedItems();
    bool removeItems(QString );
    bool removeSelectedItems();
    bool closeItems(QString );
    bool closeSelectedItems();
    bool saveItems(QString );
    bool saveSelectedItems();

    bool addPItems(QString, WImage* );
    QPair<QString, WImage*> takePItem(QString );
    QList<QPair<QString, WImage*> > takeSelectedPItems();
    bool removePItems(QString );
    bool removeSelectedPItems();
    bool movePItems(QString );
    bool moveSelectedPItems();
    bool closePItems(QString );
    bool closeSelectedPItems();
    bool savePItems(QString );
    bool saveSelectedPItems();

    // process related
public slots:
    WImage* activatedImage();

    void callToGrayScale();
    void callToBinary();

    void callNegOps();
    void callArithmOps();

    void callMove();
    void callFlipHorizontal();
    void callFlipVertical();
    void callRotate();
    void callScaleUp();
    void callScaleDown();

    void callScaleGrayExp();
    void callHistogramEqualize();

    void callFFTOps();
    void callFFTAndIFFTOps(); // 无事发生

    void callMeanFilter();
    void callGaussianFilter();
    void callMedianFilter();

    void callRobertsDetect();
    void callSobelDetect();
    void callPrewittDetect();
    void callLaplaceDetect();

    void callKMeansClustering();
    void callLaplaceSharpen();

public:
    DiscriptorExample d;

public slots:
    // 项数重构
    void callRefactor();
    void showRefactorExampleImage();
    void showRefactoredExampleImage();

public slots: // additional opeartion
    void respAddItem(QString name, WImage* image);
    void respChangeItem(QString name, WImage* image);
    void respDisplayItem(WImage* image);


//public slots:
//    void closePanel(WImage* );
private slots:
    void on_listFiles_customContextMenuRequested(const QPoint &pos);
    void on_clickDown_clicked();
    void on_clickUp_clicked();
};
#endif // MAINWINDOW_H
