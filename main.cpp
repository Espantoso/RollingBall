#include "mainwindow.h"
#include <QApplication>
//#include "qml-box2d/box2dplugin.h"

class test
{
    int tst;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
