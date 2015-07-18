#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T13:51:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LTPCWS_UI
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    config.cpp

HEADERS  += widget.h \
    config.h
CONFIG += c++11

RESOURCES += \
    res.qrc

RC_FILE += ltpcws.rc

DISTFILES += \
    ltpcws.rc \
    cws_bin/win32/msvcp120.dll \
    cws_bin/win32/msvcr120.dll \
    cws_bin/win32/otcws.exe \
    cws_bin/win32/otcws-customized.exe \
    cws_bin/osx/otcws \
    cws_bin/osx/otcws-customized \
    cws_bin/linux32/otcws \
    cws_bin/linux32/otcws-customized
