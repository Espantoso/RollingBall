#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T12:56:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../phy/Box2D/
LIBS += -L"../phy/Box2D"
LIBS += -lBox2D

TARGET = RollingBall
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    myqgraphicsview.cpp \
    ball.cpp \
    wall.cpp

HEADERS  += mainwindow.h \
    field.h \
    myqgraphicsview.h \
    ball.h \
    wall.h

FORMS    += mainwindow.ui

