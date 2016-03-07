#include "myqgraphicsview.h"
#include "mainwindow.h"
#include "field.h"
#include "ball.h"
#include <QPointF>
#include <QTextDocument>
#include <QTimer>
#include "iostream"
Ball *ball;
QGraphicsLineItem *lineLeft;
QGraphicsLineItem *lineRight;
QGraphicsLineItem *lineTop;
QGraphicsLineItem *lineBottom;
QTimer *timer=nullptr;
bool isInMenu, isInMainMenu, isInPauseMenu;
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
    ball=new Ball(QPointF(200, 100), 0);
    connect(this, SIGNAL(signal_moveToNextPosition()), ball, SLOT(slot_moveToNextPosition()));
    scene->addItem(ball);
    timer->start(50);
    lineLeft = scene->addLine(0, 0, 0, 500);
    lineRight = scene->addLine(500, 0, 500, 500);
    lineTop = scene->addLine(0, 0, 500, 0);
    lineBottom = scene->addLine(0, 500, 500, 500);
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
    timer->start(50);
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
            //StartGame(scene, this);//После добавления шара тут должна быть другая функция
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
}
void PauseMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=true;
    delete scene;
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
void MyQGraphicsView::slot_timerOut()
{
    if(ball->collidesWithItem(lineLeft))
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
    emit signal_moveToNextPosition();
}
