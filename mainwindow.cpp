#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "field.h"
#include <QGraphicsTextItem>
#include <QTextDocument>
void StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    scene->setBackgroundBrush(Qt::black);
    //scene->addRect(0, 0, 5*ui->graphicsView->width(), 12*ui->graphicsView->height(), QPen(Qt::white), QBrush(Qt::white));
    QString str_field[WIDTH][HEIGHT];
    for(int i=0; i<5; i++)
        for(int j=0; j<10; j++)
            str_field[i][j]="empty";
    Field field(str_field, 5, 10);
    field.draw(5*graphicsView->width(), 17*graphicsView->height(), scene);
    graphicsView->show();
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene *scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);
    scene->addRect(0, 50, 3*ui->graphicsView->width(), 2*ui->graphicsView->height(), QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, -50, 3*ui->graphicsView->width(), 2*ui->graphicsView->height(), QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *StartGame=new QGraphicsTextItem("Start game");
    StartGame->setFont(QFont("times", 26));
    StartGame->setPos(0, -50);
    StartGame->setTextWidth(3*ui->graphicsView->width());
    StartGame->document()->setPageSize(QSizeF(3*ui->graphicsView->width(), 2*ui->graphicsView->height()));
    StartGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(StartGame);
    QGraphicsTextItem *Quit=new QGraphicsTextItem("Quit");
    Quit->setFont(QFont("times", 26));
    Quit->setPos(0, 50);
    Quit->setTextWidth(3*ui->graphicsView->width());
    Quit->document()->setPageSize(QSizeF(3*ui->graphicsView->width(), 2*ui->graphicsView->height()));
    Quit->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Quit);
}
MainWindow::~MainWindow()
{
    delete ui;
}
