#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "field.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene *scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);
    scene->setBackgroundBrush(Qt::black);
    //scene->addRect(0, 0, 5*ui->graphicsView->width(), 12*ui->graphicsView->height(), QPen(Qt::white), QBrush(Qt::white));
    QString str_field[WIDTH][HEIGHT];
    for(int i=0; i<5; i++)
        for(int j=0; j<10; j++)
            str_field[i][j]="empty";
    Field field(str_field, 5, 10);
    field.draw(5*ui->graphicsView->width(), 20*ui->graphicsView->height(), scene);
    ui->graphicsView->show();
}
MainWindow::~MainWindow()
{
    delete ui;
}
