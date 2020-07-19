QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lib/discriptorexample.cpp \
    lib/libcaller.cpp \
    lib/ops/arithmops.cpp \
    lib/ops/basicops.cpp \
    lib/ops/fftops.cpp \
    lib/ops/filterops.cpp \
    lib/ops/geometryops.cpp \
    lib/ops/kmeansclusterops.cpp \
    lib/ops/nongeoops.cpp \
    lib/ops/ops.cpp \
    main.cpp \
    mainwindow.cpp \
    widgets/arithmdialog.cpp \
    widgets/qimgsubwindow.cpp \
    wimage.cpp

HEADERS += \
    lib/discriptorexample.h \
    lib/libcaller.h \
    lib/ops/arithmops.h \
    lib/ops/basicops.h \
    lib/ops/fftops.h \
    lib/ops/filterops.h \
    lib/ops/geometryops.h \
    lib/ops/kmeansclusterops.h \
    lib/ops/nongeoops.h \
    lib/ops/ops.h \
    mainwindow.h \
    widgets/arithmdialog.h \
    widgets/qimgsubwindow.h \
    wimage.h

FORMS += \
    mainwindow.ui \
    widgets/arithmdialog.ui \
    widgets/qimgsubwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    sources.qrc

DISTFILES += \
    resources/arrow-down.ico \
    resources/back.jpg \
    resources/back.png \
    resources/down.ico \
    resources/up.ico
