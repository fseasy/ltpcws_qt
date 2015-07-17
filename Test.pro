#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T13:51:38
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    config.cpp

HEADERS  += widget.h \
    config.h
CONFIG += c++11

RESOURCES += \
    res.qrc
