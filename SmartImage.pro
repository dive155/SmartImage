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
    curver.cpp \
    sharper.cpp \
    edger.cpp \
    colorer.cpp \
    bilaterator.cpp \
    helpwindow.cpp

HEADERS  += mainwindow.h \
    simpleaver.h \
    adaptaver.h \
    curver.h \
    sharper.h \
    edger.h \
    colorer.h \
    bilaterator.h \
    helpwindow.h

FORMS    += mainwindow.ui \
    simpleaver.ui \
    adaptaver.ui \
    curver.ui \
    sharper.ui \
    edger.ui \
    colorer.ui \
    bilaterator.ui \
    helpwindow.ui

RESOURCES += \
    placeholders.qrc

RC_ICONS = si.ico
