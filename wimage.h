#ifndef WIMAGE_H
#define WIMAGE_H

#pragma once

#include <QImage>

class QImgSubWindow;

class WImage : public QObject, public QImage
{
    Q_OBJECT
public:

    QImgSubWindow* panel; // 指向显示窗口的指针，避免加载多次
    explicit WImage(bool );
    explicit WImage(QString ,bool );
    explicit WImage(QString, bool, const QImage&);
    explicit WImage(int, int, Format f = Format_RGB32);
    explicit WImage(const QImage& );
    WImage(const WImage& );
    ~WImage();

    const QString& getName() const;
    bool isOpened() const;
    void setName(QString );
    void setOpend(bool );

    WImage* deepcopy()const;

//    bool convertToFormat(Format, Qt::ImageConversionFlag = Qt::AutoColor);
    bool operator=(const WImage& );

    int isGrayscale() const;
    int isBinary() const;
    int isGrayscale() ;
    int isBinary() ;

private:
    QString name;
    bool opened;
    int gray;
    int binary;

    void init(QString, bool, QImgSubWindow*, int, int);

signals:
    void askClosePanel(QImgSubWindow* );

};

#endif // WIMAGE_H
