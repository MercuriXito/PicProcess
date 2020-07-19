#ifndef QIMGSUBWINDOW_H
#define QIMGSUBWINDOW_H

#include <QWidget>
#include <QMdiSubWindow>

#include "wimage.h"

namespace Ui {
class QImgSubWindow;
}

class QImgSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    explicit QImgSubWindow(WImage* image, QWidget *parent = nullptr);
    ~QImgSubWindow();

    void draw();
    const QString getImageName();
    const bool isOpened();

private:
    Ui::QImgSubWindow *ui;
    WImage* image;

    int iw, ih; // 图像最初的高宽
    int x, y, w, h; // 动态的起点和高宽

    void init();

    void closeEvent(QCloseEvent *closeEvent) override;

signals:
    void sendClose(WImage* );
};

#endif // QIMGSUBWINDOW_H
