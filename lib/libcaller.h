#ifndef LIBCALLER_H
#define LIBCALLER_H

#include <QObject>

class WImage;

class LibCaller: public QObject
{
    Q_OBJECT
public:
    LibCaller();

    void askAddImage(WImage* );
    void askChangeImage(QString, WImage* );
    void askDisplayImage(QString );

signals:
    void callAddImage(WImage* );
    void callChangeIamge(QString , WImage* );
    void callDisplayImage(QString );
};

#endif // LIBCALLER_H
