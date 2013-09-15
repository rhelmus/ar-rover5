#-------------------------------------------------
#
# Project created by QtCreator 2013-09-14T16:28:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rover5-control
TEMPLATE = app


SOURCES += main.cpp\
    ../../control-base/tcpclientinterface.cpp \
    ../../control-base/scaledpixmapwidget.cpp \
    ../../control-base/rover5control.cpp \
    ../../control-base/numstatwidget.cpp \
    ../../control-base/mapscene.cpp \
    drivewidget.cpp \
    ../../shared/tcputils.cpp \
    ../../control-base/utils.cpp

HEADERS  += \
    ../../control-base/utils.h \
    ../../control-base/tcpclientinterface.h \
    ../../control-base/scaledpixmapwidget.h \
    ../../control-base/rover5control.h \
    ../../control-base/numstatwidget.h \
    ../../control-base/mapscene.h \
    drivewidget.h \
    ../../shared/shared.h \
    ../../shared/tcputils.h

CONFIG += mobility
MOBILITY = 

OTHER_FILES += \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-et/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-el/strings.xml \
    android/version.xml

RESOURCES += \
    ../../control-base/resources.qrc \
    android_res.qrc

