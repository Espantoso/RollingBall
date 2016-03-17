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
    for(float i=96; i<134; i=i+4)
        wall->addWallPoint(scene, 200, i-20);
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
	else
	{
		wall->addWallPoint(scene, pt.x(), pt.y());
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

	if (key==Qt::Key_Space)
	{
		timer->start(5);
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
qreal getAngle(QPointF BallPos, QPointF WallPos, double dir)
{
    double alpha=(qAtan(fabs(WallPos.y()-BallPos.y())/fabs(WallPos.x()-BallPos.x()))* 180)/M_PI;
    double beta=qAcos(qCos(alpha*M_PI/180)*qCos(dir*M_PI/180)+qSin(alpha*M_PI/180)*qSin(dir*M_PI/180));
    beta=beta*180/M_PI;
    if(fabs(beta)>90)
        return dir;
    double gamma=alpha-dir;
    return dir+180+2*gamma;
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
            float Ballx=ball->pos().x();
            float Bally=ball->pos().y();
            float min=1000000;
            QPointF NearestPoint;
            for(int j=0; j<wall->getWallLength(); j++)
            {
                float Wallx=wall->getWallPointCenter(j).x();
                float Wally=wall->getWallPointCenter(j).y();
                float distance=qSqrt((Ballx-Wallx)*(Ballx-Wallx)+(Bally-Wally)*(Bally-Wally));
                if(distance<min)
                {
                    min=distance;
                    NearestPoint=wall->getWallPointCenter(j);
                }
            }
            qreal angle = getAngle(ball->pos(), NearestPoint, ball->getDir());
            ball->setDir(angle);
        }
    }
    emit signal_moveToNextPosition();
}
