#include "libcaller.h"

LibCaller::LibCaller()
{

}

void LibCaller::askAddImage(WImage *image){
    emit callAddImage(image);
}

void LibCaller::askChangeImage(QString name, WImage *image){
    emit callChangeIamge(name, image);
}

void LibCaller::askDisplayImage(QString name){
    emit callDisplayImage(name);
}
