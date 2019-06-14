#-------------------------------------------------
#
# Project created by QtCreator 2019-06-07T17:21:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = video-snapcut
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        drawpanel.cpp \
        drawscene.cpp \
        main.cpp \
        mainwindow.cpp \
        myfold.cpp \
        myline.cpp \
        ../tester/opencvtester.cpp \
        ../tester/graphcuttester.cpp \
        shape.cpp

HEADERS += \
        drawpanel.h \
        drawscene.h \
        mainwindow.h \
        myfold.h \
        myline.h \
        ../tester/opencvtester.h \
        ../tester/graphcuttester.h \
        ../tester/gcgraph.hpp \
        shape.h

FORMS += \
        mainwindow.ui

## for OPENCV

INCLUDEPATH += \
        /usr/local/include
LIBS += -L /usr/local/lib \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_imgcodecs \
        -lopencv_features2d \
        -lopencv_objdetect \
        -lopencv_calib3d \
        -lopencv_video \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
