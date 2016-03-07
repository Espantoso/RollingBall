#include "myqgraphicsview.h"
#include "mainwindow.h"
#include "field.h"
#include "ball.h"
#include "wall.h"
#include <QPointF>
#include <QTextDocument>
#include <QTimer>
#include "iostream"
#include "qmath.h"
Ball *ball;
QGraphicsLineItem *lineLeft;
QGraphicsLineItem *lineRight;
QGraphicsLineItem *lineTop;
QGraphicsLineItem *lineBottom;
QGraphicsScene *GameScene;
QTimer *timer=nullptr;
QGraphicsRectItem *finish;
Wall *wall=new Wall(5, Qt::gray);
bool isInMenu, isInMainMenu, isInPauseMenu, isInCompletedMenu=false;
void MyQGraphicsView::StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=false;
    delete scene;
    scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, 500, 500);
    graphicsView->setScene(scene);
    //scene->setBackgroundBrush(Qt::black);
    /*QString str_field[WIDTH][HEIGHT];
	for(int i=0; i<PLANE_WIDTH; i++)
		for(int j=0; j<PLANE_HEIGHT; j++)
            str_field[i][j]="empty";
    Field field(str_field, PLANE_WIDTH, PLANE_HEIGHT);*/
    //field.draw(500, 550, scene);
    if(!timer)
        timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timerOut()), Qt::UniqueConnection);
    ball=new Ball(QPointF(100, 100), 0);
    connect(this, SIGNAL(signal_moveToNextPosition()), ball, SLOT(slot_moveToNextPosition()));
    scene->addItem(ball);
    lineLeft = scene->addLine(0, 0, 0, 500);
    lineRight = scene->addLine(500, 0, 500, 500);
    lineTop = scene->addLine(0, 0, 500, 0);
    lineBottom = scene->addLine(0, 500, 500, 500);
    finish=scene->addRect(200, 400, 50, 50, QPen(Qt::green), QBrush(Qt::green));
    wall->clear(scene);//Линия, которую должен добавлять пользователь(добавлена для тестирования)
    wall->addWallPoint(scene, 200, 100);
    wall->addWallPoint(scene, 201, 101);
    wall->addWallPoint(scene, 202, 102);
    wall->addWallPoint(scene, 203, 103);
    wall->addWallPoint(scene, 204, 103);
    wall->addWallPoint(scene, 205, 102);
    wall->addWallPoint(scene, 206, 101);
    wall->addWallPoint(scene, 207, 100);
    wall->addWallPoint(scene, 199, 99);
    wall->addWallPoint(scene, 198, 98);
    wall->addWallPoint(scene, 197, 97);
    wall->addWallPoint(scene, 196, 96);
    /*QGraphicsEllipseItem *e2=scene->addEllipse(201, 101, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e2);
    QGraphicsEllipseItem *e3=scene->addEllipse(202, 102, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e3);
    QGraphicsEllipseItem *e4=scene->addEllipse(203, 103, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e4);
    QGraphicsEllipseItem *e5=scene->addEllipse(204, 103, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e5);
    QGraphicsEllipseItem *e6=scene->addEllipse(205, 102, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e6);
    QGraphicsEllipseItem *e7=scene->addEllipse(206, 101, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e7);
    QGraphicsEllipseItem *e8=scene->addEllipse(207, 100, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e8);
    QGraphicsEllipseItem *e9=scene->addEllipse(199, 99, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e9);
    QGraphicsEllipseItem *e10=scene->addEllipse(198, 98, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e10);
    QGraphicsEllipseItem *e11=scene->addEllipse(197, 97, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e11);
    QGraphicsEllipseItem *e12=scene->addEllipse(196, 96, 5, 5, QPen(Qt::gray), QBrush(Qt::gray));
    wall.append(e12);*/
    timer->start(5);
}
void MainMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    delete scene;
    scene = new QGraphicsScene;
    graphicsView->setScene(scene);
    scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, -50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
	QGraphicsTextItem *StartGame=new QGraphicsTextItem("Start");
	StartGame->setFont(QFont("helvetica", 20));
    StartGame->setPos(0, -50);
    StartGame->setTextWidth(300);
    StartGame->document()->setPageSize(QSizeF(300, 50));
    StartGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(StartGame);
	QGraphicsTextItem *Quit=new QGraphicsTextItem("Exit");
	Quit->setFont(QFont("helvetica", 20));
    Quit->setPos(0, 50);
    Quit->setTextWidth(300);
    Quit->document()->setPageSize(QSizeF(300, 50));
    Quit->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Quit);
}
MyQGraphicsView::MyQGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    isInMenu=true;
    isInMainMenu=true;
    isInPauseMenu=false;
    scene = new QGraphicsScene;
    this->setScene(scene);
    MainMenu(scene, this);
    this->setInteractive(true);
}
void MyQGraphicsView::BackToGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=false;
    scene = GameScene;
    graphicsView->setScene(scene);
    //scene->setBackgroundBrush(Qt::black);
    /*QString str_field[WIDTH][HEIGHT];
    for(int i=0; i<PLANE_WIDTH; i++)
        for(int j=0; j<PLANE_HEIGHT; j++)
            str_field[i][j]="empty";
    Field field(str_field, PLANE_WIDTH, PLANE_HEIGHT);*/
    //field.draw(500, 550, scene);
    if(!timer)
        timer = new QTimer(this);
    timer->start(5);
    lineLeft = scene->addLine(0, 0, 0, 500);
    lineRight = scene->addLine(500, 0, 500, 500);
    lineTop = scene->addLine(0, 0, 500, 0);
    lineBottom = scene->addLine(0, 500, 500, 500);
}
void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {
        if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-50)&&(pt.y()<0))//Начать игру
        {
            isInMainMenu=false;
            StartGame(scene, this);
        }
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>50)&&(pt.y()<100))//Выход
            exit(0);
    }
    else if(isInPauseMenu)
    {
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-125)&&((pt.y()<-75)))//Вернуться к игре
        {
            isInPauseMenu=false;
            BackToGame(scene, this);
        }
        else if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-25)&&((pt.y()<25)))//Главное меню
        {
            isInMainMenu=true;
            isInPauseMenu=false;
            MainMenu(scene, this);
        }
        else if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>75)&&((pt.y()<125)))//Выход
            exit(0);
    }
    else if(isInCompletedMenu)
    {
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>50)&&((pt.y()<100)))//Главное меню
        {
            isInCompletedMenu=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
    }
}
void PauseMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=true;
    GameScene=scene;
    scene = new QGraphicsScene;
    graphicsView->setScene(scene);
    scene->addRect(0, -125, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, -25, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, 75, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
	QGraphicsTextItem *ReturnToGame=new QGraphicsTextItem("Back to game");
	ReturnToGame->setFont(QFont("helvetica", 20));
    ReturnToGame->setPos(0, -125);
    ReturnToGame->setTextWidth(300);
    ReturnToGame->document()->setPageSize(QSizeF(300, 50));
    ReturnToGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(ReturnToGame);
	QGraphicsTextItem *MainMenu=new QGraphicsTextItem("Main menu");
	MainMenu->setFont(QFont("helvetica", 20));
    MainMenu->setPos(0, -25);
    MainMenu->setTextWidth(300);
    MainMenu->document()->setPageSize(QSizeF(300, 50));
    MainMenu->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(MainMenu);
	QGraphicsTextItem *Quit=new QGraphicsTextItem("Exit");
	Quit->setFont(QFont("helvetica", 20));
    Quit->setPos(0, 75);
    Quit->setTextWidth(300);
    Quit->document()->setPageSize(QSizeF(300, 50));
    Quit->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Quit);
}
void MyQGraphicsView::keyPressEvent(QKeyEvent *e)
{
    int key=e->key();
    if((key==Qt::Key_Escape)&&(!isInMenu))
    {
        timer->stop();
        isInPauseMenu=true;
        PauseMenu(scene, this);
    }
}
qreal getAngle(QPointF p1, QPointF p2)
{
    QPointF point = p1;
    QPointF center = p2;

    qreal tgAngle = 0;
    qreal deg = 0;
    qreal k = 0;

    qreal x1 = ceil(center.x());
    qreal y1 = ceil(center.y());
    qreal x2 = ceil(point.x());
    qreal y2 = ceil(point.y());

    if(x1 == x2)
    {
        if(y2 > y1) deg = 0;
        if(y2 <= y1) deg = 180;
    }
    else
    {
        tgAngle = qAtan((y2 - y1)/(x2 - x1));
        deg = (tgAngle * 180)/M_PI;
    }

    if(deg < 0) deg = 360 + deg;
    if(deg > 360) deg = deg - 360;

    if((x2 > x1) && (y2 >= y1)) k = 90;
    if((x2 > x1) && (y2 <= y1)) k = 90;
    if((x2 < x1) && (y2 <= y1)) k = 270;
    if((x2 < x1) && (y2 >= y1)) k = 270;
    return -(deg - k);
}
void MyQGraphicsView::Completed()
{
    isInCompletedMenu=true;
    timer->stop();
    isInMenu=true;
    scene = new QGraphicsScene;
    this->setScene(scene);
    QGraphicsTextItem *Completed=new QGraphicsTextItem("Completed!");
    Completed->setFont(QFont("helvetica", 40));
    Completed->setPos(0, -50);
    Completed->setTextWidth(300);
    Completed->document()->setPageSize(QSizeF(300, 100));
    Completed->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Completed);
    scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *MainMenu=new QGraphicsTextItem("Main menu");
    MainMenu->setFont(QFont("helvetica", 20));
    MainMenu->setPos(0, 50);
    MainMenu->setTextWidth(300);
    MainMenu->document()->setPageSize(QSizeF(300, 50));
    MainMenu->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(MainMenu);
}
void MyQGraphicsView::slot_timerOut()
{
    if(ball->collidesWithItem(finish))
        Completed();
    else if(ball->collidesWithItem(lineLeft))
    {
        qreal angle = 90 + (90 - ball->getDir());
        ball->setDir(angle);
    }
    else if(ball->collidesWithItem(lineRight))
    {
        qreal angle = 270 + (270 - ball->getDir());
        ball->setDir(angle);
    }
    else if(ball->collidesWithItem(lineTop))
    {
        ball->setDir(- ball->getDir());
    }
    else if(ball->collidesWithItem(lineBottom))
    {
        qreal angle = 180 + (180 - ball->getDir());
        ball->setDir(angle);
    }
    for(int i = 0; i < wall->getWallLength(); i++)
    {
        if(ball->collidesWithItem(wall->getWallPoint(i)))
        {
            qreal angle = getAngle(ball->pos(), wall->getWallPoint(i)->pos());
            ball->setDir(angle);
        }
    }
    emit signal_moveToNextPosition();
}
