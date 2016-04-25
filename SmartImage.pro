#-------------------------------------------------
#
# Project created by QtCreator 2016-04-25T18:30:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartImage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simpleaver.cpp \
    adaptaver.cpp \
    curver.cpp

HEADERS  += mainwindow.h \
    simpleaver.h \
    adaptaver.h \
    curver.h

FORMS    += mainwindow.ui \
    simpleaver.ui \
    adaptaver.ui \
    curver.ui
