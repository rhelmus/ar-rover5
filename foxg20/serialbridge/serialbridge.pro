#-------------------------------------------------
#
# Project created by QtCreator 2013-08-26T22:33:51
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT += network

TARGET = serialbridge
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += serialport

TEMPLATE = app


SOURCES += main.cpp \
    serialbridge.cpp \
    tcpserver.cpp

HEADERS += \
    serialbridge.h \
    ../../shared/shared.h \
    tcpserver.h
