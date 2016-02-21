#include "myqgraphicsview.h"
#include "mainwindow.h"
//#include "scene.h"
#include <QPointF>
#include <QTextDocument>
MyQGraphicsView::MyQGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    scene = new QGraphicsScene;
    this->setScene(scene);
    scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, -50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *StartGame=new QGraphicsTextItem("Начать игру");
    StartGame->setFont(QFont("times", 26));
    StartGame->setPos(0, -50);
    StartGame->setTextWidth(300);
    StartGame->document()->setPageSize(QSizeF(300, 50));
    StartGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(StartGame);
    QGraphicsTextItem *Quit=new QGraphicsTextItem("Выход");
    Quit->setFont(QFont("times", 26));
    Quit->setPos(0, 50);
    Quit->setTextWidth(300);
    Quit->document()->setPageSize(QSizeF(300, 50));
    Quit->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Quit);
    this->setInteractive(true);
}
void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    scene->addRect(0, 10, 20, 40);
    if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>50)&&((pt.y()<100)))
        exit(0);
}
