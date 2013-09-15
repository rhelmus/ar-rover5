#-------------------------------------------------
#
# Project created by QtCreator 2012-12-27T14:43:53
#
#-------------------------------------------------

QT       += core gui network

CONFIG += mobility
MOBILITY = connectivity

TARGET = rover5-control
TEMPLATE = app


SOURCES += main.cpp\
    ../../shared/tcputils.cpp \
    btinterface.cpp \
    drivewidget.cpp \
    ../../control-base/utils.cpp \
    ../../control-base/tcpclientinterface.cpp \
    ../../control-base/scaledpixmapwidget.cpp \
    ../../control-base/rover5control.cpp \
    ../../control-base/numstatwidget.cpp \
    ../../control-base/mapscene.cpp

HEADERS  +=  ../../shared/tcputils.h \
    ../../shared/shared.h \
    btinterface.h \
    drivewidget.h \
    ../../control-base/utils.h \
    ../../control-base/tcpclientinterface.h \
    ../../control-base/scaledpixmapwidget.h \
    ../../control-base/rover5control.h \
    ../../control-base/numstatwidget.h \
    ../../control-base/mapscene.h

OTHER_FILES +=

RESOURCES += \
    ../../control-base/resources.qrc \
    pc_res.qrc
