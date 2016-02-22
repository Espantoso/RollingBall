#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T12:56:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RollingBall
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    myqgraphicsview.cpp

HEADERS  += mainwindow.h \
    field.h \
    myqgraphicsview.h

FORMS    += mainwindow.ui

