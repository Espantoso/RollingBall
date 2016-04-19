#-------------------------------------------------
#
# Project created by QtCreator 2011-10-31T18:02:42
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH += ../phy/Box2D/
LIBS += -L"../phy/Box2D"
LIBS += -lBox2D


TARGET = phy
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
    ball.cpp \
    mainwindow.cpp \
    myqgraphicsview.cpp \
    wall.cpp

HEADERS  += phy.h \
    ball.h \
    mainwindow.h \
    myqgraphicsview.h \
    field.h \
    wall.h

FORMS += \
    mainwindow.ui
