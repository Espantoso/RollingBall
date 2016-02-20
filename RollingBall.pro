#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T12:56:26
#
#-------------------------------------------------
include(qml-box2d/box2d-static.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RollingBall
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    qml-box2d/Box2D/Box2D/Rope/b2Rope.cpp

HEADERS  += mainwindow.h \
    field.h

FORMS    += mainwindow.ui

DISTFILES += \
    qml-box2d/box2d-static.pri
INCLUDEPATH += ../qml-box2d/Box2D
LIBS += -L"../qml-box2d/Box2D/Build/Box2D"
LIBS += -lBox2D
