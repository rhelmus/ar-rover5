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
        rover5control.cpp \
    ../../shared/tcputils.cpp \
    btinterface.cpp

HEADERS  += rover5control.h \
    ../../shared/tcputils.h \
    ../../shared/shared.h \
    btinterface.h
