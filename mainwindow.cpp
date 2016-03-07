#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "field.h"
#include "myqgraphicsview.h"
#include <QGraphicsTextItem>
#include <QTextDocument>
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}
