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
#include <QFile>
#include <QVector>
#include <QThread>
#include <QString>
#include <QTextStream>
Ball *ball;
QGraphicsLineItem *lineLeft;
QGraphicsLineItem *lineRight;
QGraphicsLineItem *lineTop;
QGraphicsLineItem *lineBottom;
QGraphicsScene *GameScene;
QTimer *timer=nullptr;
QGraphicsRectItem *finish;
const float WallWidth=5, DistBetweenWallEl=3, BallR=20;
float PrevPointX=-100, PrevPointY;
QPointF BallStartPos=QPointF(100, 100);
float BallStartDir=270+45;
Wall *wall=new Wall(WallWidth, Qt::gray);
bool isInMenu, isInMainMenu, isInPauseMenu, isInCompletedMenu=false, LeftButtonDown=false;
bool isTimeLaunched, AddMode, isInChooseLevelMenu=false;
QVector <QByteArray> level_names;
class LevelListCreator:public QThread
{
public:
    void run()
    {
        int i=1;
        while(true)
        {
            QFile file(QString("levels/level")+QString::number(i)+QString(".txt"));
            if(file.open(QIODevice::ReadOnly))
            {
                QByteArray level_name=file.readLine();
                level_names.append(level_name);
                file.close();
            }
            else
                break;
            i++;
        }
    }
};
class LevelListDrawer:public QThread
{
private:
    QGraphicsScene* scene;
public:
    LevelListDrawer::LevelListDrawer(QGraphicsScene *scene)
    {
        this->scene=scene;
    }
    void run()
    {
        int start=0;
        while(isInChooseLevelMenu)
        {
            int length=level_names.length();
            for(int i=start; i<length; i++)
            {
                if(i%2==0)
                {
                    scene->addRect(0, 50*i, 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                }
                else
                {
                    scene->addRect(300, 50*(i-1), 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                }
            }
            start=length;
        }
    }
};
void LaunchTime(QGraphicsScene *scene)
{
    timer->start(5);
    isTimeLaunched=true;
    scene->addRect(510, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Restart=new QGraphicsTextItem("Restart");
    Restart->setFont(QFont("helvetica", 20));
    Restart->setPos(407, 45);
    Restart->setTextWidth(300);
    Restart->document()->setPageSize(QSizeF(300, 50));
    Restart->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Restart);
    scene->addRect(510, 90, 200, 500, QPen(Qt::white), QBrush(Qt::white));
}
void MyQGraphicsView::StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    AddMode=true;
    isInMenu=false;
    delete scene;
    scene = new QGraphicsScene;
    this->scene=scene;
    scene->setSceneRect(0, 0, 500, 500);
    graphicsView->setScene(scene);
    if(!timer)
        timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timerOut()), Qt::UniqueConnection);
    ball=new Ball(BallStartPos, BallStartDir);
    connect(this, SIGNAL(signal_moveToNextPosition()), ball, SLOT(slot_moveToNextPosition()));
    scene->addItem(ball);
    lineLeft = scene->addLine(0, 0, 0, 500);
    lineRight = scene->addLine(500, 0, 500, 500);
    lineTop = scene->addLine(0, 0, 500, 0);
    lineBottom = scene->addLine(0, 500, 500, 500);
    finish=scene->addRect(200, 400, 50, 50, QPen(Qt::green), QBrush(Qt::green));
    wall->DeleteFromMemory(scene);
    scene->addRect(510, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Start=new QGraphicsTextItem("Start");
    Start->setFont(QFont("helvetica", 20));
    Start->setPos(395, 45);
    Start->setTextWidth(300);
    Start->document()->setPageSize(QSizeF(300, 50));
    Start->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Start);
    scene->addRect(510, 90, 30, 30, QPen(Qt::red), QBrush(Qt::gray));
    scene->addRect(550, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addRect(510, 140, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *RemoveAll=new QGraphicsTextItem("Remove all");
    RemoveAll->setFont(QFont("helvetica", 12));
    RemoveAll->setPos(405, 141);
    RemoveAll->setTextWidth(300);
    RemoveAll->document()->setPageSize(QSizeF(300, 50));
    RemoveAll->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(RemoveAll);
}
void MyQGraphicsView::ChooseLevel(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInChooseLevelMenu=true;
    scene = new QGraphicsScene;
    this->scene=scene;
    scene->setSceneRect(0, 0, 500, 500);
    graphicsView->setScene(scene);
    LevelListDrawer *level_list_drawer=new LevelListDrawer(scene);
    level_list_drawer->start();
}
void RestartGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=false;
    isTimeLaunched=false;
    timer->stop();
    ball->setPos(BallStartPos);
    ball->setDir(BallStartDir);
    scene->addRect(510, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Start=new QGraphicsTextItem("Start");
    Start->setFont(QFont("helvetica", 20));
    Start->setPos(395, 45);
    Start->setTextWidth(300);
    Start->document()->setPageSize(QSizeF(300, 50));
    Start->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Start);
    scene->addRect(510, 90, 30, 30, QPen(Qt::red), QBrush(Qt::gray));
    scene->addRect(550, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addRect(510, 140, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *RemoveAll=new QGraphicsTextItem("Remove all");
    RemoveAll->setFont(QFont("helvetica", 12));
    RemoveAll->setPos(405, 141);
    RemoveAll->setTextWidth(300);
    RemoveAll->document()->setPageSize(QSizeF(300, 50));
    RemoveAll->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(RemoveAll);
}
void MainMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    /*QFile *file;
    for(int i=2; i<=10; i++)
    {
        file=new QFile(QString("levels/level")+QString::number(i)+QString(".txt"));
        file->open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(file);
        writeStream << "level "+QString::number(i); // Посылаем строку в поток для записи
        file->close();
    }*/
    isTimeLaunched=false;
    delete scene;
    scene = new QGraphicsScene;
    graphicsView->setScene(scene);
    scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    scene->addRect(0, -50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *StartGame=new QGraphicsTextItem("Choose level");
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
    LevelListCreator *level_list_creator=new LevelListCreator();
    level_list_creator->start();
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
    if(isTimeLaunched)
        timer->start(5);
}
void SelectAddWallTool(QGraphicsScene *scene)
{
    AddMode=true;
    scene->addRect(510, 90, 30, 30, QPen(Qt::red), QBrush(Qt::gray));
    scene->addRect(550, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
}
void SelectRemoveWallTool(QGraphicsScene *scene)
{
    AddMode=false;
    scene->addRect(510, 90, 30, 30, QPen(Qt::black), QBrush(Qt::gray));
    scene->addRect(550, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
}
void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    LeftButtonDown=true;
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {
        if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-50)&&(pt.y()<0))//Начать игру
        {
            isInMainMenu=false;
            ChooseLevel(scene, this);
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
    else if(isTimeLaunched)
    {
        if((pt.x()>510)&&(pt.x()<605)&&(pt.y()>50)&&(pt.y()<80))
        {
            RestartGame(scene, this);
        }
    }
    else
    {
        if((pt.x()>510)&&(pt.x()<605)&&(pt.y()>50)&&(pt.y()<80))
        {
            LaunchTime(scene);
            return;
        }
        else if((pt.x()>510)&&(pt.x()<540)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectAddWallTool(scene);
            return;
        }
        else if((pt.x()>550)&&(pt.x()<580)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectRemoveWallTool(scene);
            return;
        }
        else if((pt.x()>510)&&(pt.x()<605)&&(pt.y()>140)&&(pt.y()<170))
        {
            for(int j=0; j<wall->getWallLength();)
            {
                wall->deletePoint(scene, j);
            }
            return;
        }
        if(AddMode)
        {
            float distance;
            for(int j=0; j<wall->getWallLength(); j++)
            {
                float Wallx=wall->getWallPointCenter(j).x();
                float Wally=wall->getWallPointCenter(j).y();
                distance=qSqrt((pt.x()-Wallx)*(pt.x()-Wallx)+(pt.y()-Wally)*(pt.y()-Wally));
                if(distance<DistBetweenWallEl)
                    return;
            }
            float BallX=ball->pos().x(), BallY=ball->pos().y();
            distance=qSqrt((pt.x()-BallX)*(pt.x()-BallX)+(pt.y()-BallY)*(pt.y()-BallY));
            if(distance<BallR)
                return;
            PrevPointX=pt.x();
            PrevPointY=pt.y();
            if((pt.x()>500-WallWidth)||(pt.x()<WallWidth)||(pt.y()>500-WallWidth)||(pt.y()<WallWidth))
                return;
            wall->addWallPoint(scene, pt.x(), pt.y());
        }
        else
        {
            float distance;
            for(int i=0; i<5; i++)
            {
            for(int j=0; j<wall->getWallLength(); j++)
            {
                float Wallx=wall->getWallPointCenter(j).x();
                float Wally=wall->getWallPointCenter(j).y();
                distance=qSqrt((pt.x()-Wallx)*(pt.x()-Wallx)+(pt.y()-Wally)*(pt.y()-Wally));
                if(distance<2*WallWidth)
                    wall->deletePoint(scene, j);
            }
            }
        }
	}
}
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    PrevPointX=-100;
    LeftButtonDown=false;
}
void MyQGraphicsView::mouseMoveEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    if((!isInMenu)&&(!isTimeLaunched)&&(AddMode))
    {
        float BallX=ball->pos().x(), BallY=ball->pos().y();
        float distance=qSqrt((pt.x()-BallX)*(pt.x()-BallX)+(pt.y()-BallY)*(pt.y()-BallY));
        if(distance<1.5*BallR)
            return;
        if((pt.x()>500-WallWidth)||(pt.x()<WallWidth)||(pt.y()>500-WallWidth)||(pt.y()<WallWidth))
            return;
        if(LeftButtonDown)
        {
            float distance=qSqrt((pt.x()-PrevPointX)*(pt.x()-PrevPointX)+(pt.y()-PrevPointY)*(pt.y()-PrevPointY));
            if((distance>DistBetweenWallEl)&&(PrevPointX>0))
            {
                double k=fabs(PrevPointY-pt.y())/fabs(PrevPointX-pt.x());
                if(k<=1)
                {
                    float x0=PrevPointX<pt.x()?PrevPointX:pt.x(), y0, x1, y1;
                    if(x0==pt.x())
                    {
                        y0=pt.y();
                        x0=pt.x();
                        y1=PrevPointY;
                        x1=PrevPointX;
                    }
                    else
                    {
                        y0=PrevPointY;
                        x0=PrevPointX;
                        y1=pt.y();
                        x1=pt.x();
                    }
                    if(y1<y0)
                        k=-k;
                    float steep=DistBetweenWallEl*cos(qAtan(k));
                    for(float i=x0; i<x1; i=i+steep)
                    {
                        float BallX=ball->pos().x(), BallY=ball->pos().y(), x=i, y=k*(i-x0)+y0;
                        float distance=qSqrt((x-BallX)*(x-BallX)+(y-BallY)*(y-BallY));
                        if(distance<1.5*BallR)
                        {
                            PrevPointX=-100;
                            return;
                        }
                        if((x>500-WallWidth)||(x<WallWidth)||(y>500-WallWidth)||(y<WallWidth))
                        {
                            PrevPointX=-100;
                            return;
                        }
                        wall->addWallPoint(scene, x, y);
                    }
                }
                else
                {
                    float y0=PrevPointY<pt.y()?PrevPointY:pt.y(), x0, y1, x1;
                    if(y0==pt.y())
                    {
                        y0=pt.y();
                        x0=pt.x();
                        y1=PrevPointY;
                        x1=PrevPointX;
                    }
                    else
                    {
                        y0=PrevPointY;
                        x0=PrevPointX;
                        y1=pt.y();
                        x1=pt.x();
                    }
                    if(x1<x0)
                        k=-k;
                    float steep=DistBetweenWallEl*cos(qAtan(1/k));
                    for(float i=y0; i<y1; i=i+steep)
                    {
                        float BallX=ball->pos().x(), BallY=ball->pos().y(), x=(1/k)*(i-y0)+x0, y=i;
                        float distance=qSqrt((x-BallX)*(x+x0-BallX)+(y-BallY)*(y-BallY));
                        if(distance<1.5*BallR)
                        {
                            PrevPointX=-100;
                            return;
                        }
                        if((x>500-WallWidth)||(x<WallWidth)||(y>500-WallWidth)||(y<WallWidth))
                        {
                            PrevPointX=-100;
                            return;
                        }
                        wall->addWallPoint(scene, x, y);
                    }
                }
            }
            PrevPointX=pt.x();
            PrevPointY=pt.y();
            for(int j=0; j<wall->getWallLength(); j++)
            {
                float Wallx=wall->getWallPointCenter(j).x();
                float Wally=wall->getWallPointCenter(j).y();
                distance=qSqrt((pt.x()-Wallx)*(pt.x()-Wallx)+(pt.y()-Wally)*(pt.y()-Wally));
                if(distance<DistBetweenWallEl)
                    return;
            }
            wall->addWallPoint(scene, pt.x(), pt.y());
        }
    }
    else if((!isInMenu)&&(!isTimeLaunched)&&(!AddMode))
    {
        if(LeftButtonDown)
        {
            float distance;
            for(int j=0; j<wall->getWallLength(); j++)
            {
                float Wallx=wall->getWallPointCenter(j).x();
                float Wally=wall->getWallPointCenter(j).y();
                distance=qSqrt((pt.x()-Wallx)*(pt.x()-Wallx)+(pt.y()-Wally)*(pt.y()-Wally));
                if(distance<2*WallWidth)
                    wall->deletePoint(scene, j);
            }
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
	if (key==Qt::Key_Space)
        LaunchTime(scene);
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
qreal getAngle(QPointF BallPos, QPointF WallPos, double dir)
{
    double alpha=(qAtan(fabs(WallPos.y()-BallPos.y())/fabs(WallPos.x()-BallPos.x()))* 180)/M_PI;
    if((WallPos.x()<BallPos.x())&&(WallPos.y()<BallPos.y()))
        alpha=180-alpha;
    else if((WallPos.x()<BallPos.x())&&(WallPos.y()>BallPos.y()))
        alpha=180+alpha;
    else if((WallPos.x()>BallPos.x())&&(WallPos.y()>BallPos.y()))
        alpha=360-alpha;
    double beta=qAcos(qCos(alpha*M_PI/180)*qCos(dir*M_PI/180)+qSin(alpha*M_PI/180)*qSin(dir*M_PI/180));
    beta=beta*180/M_PI;
    if(fabs(beta)>90)
        return dir;
    double gamma=alpha-dir;
    return dir+180+2*gamma;
}
void MyQGraphicsView::slot_timerOut()
{
    if(ball->collidesWithItem(finish))
        Completed();
    else if(ball->collidesWithItem(lineLeft))
    {
        if(!((ball->getDir()<90)||(ball->getDir()>270)))
        {
            qreal angle = 90 + (90 - ball->getDir());
            ball->setDir(angle);
        }
    }
    else if(ball->collidesWithItem(lineRight))
    {
        if(!((ball->getDir()>90)&&(ball->getDir()<270)))
        {
            qreal angle = 270 + (270 - ball->getDir());
            ball->setDir(angle);
        }
    }
    else if(ball->collidesWithItem(lineTop))
    {
        if(!(ball->getDir()>180))
            ball->setDir(- ball->getDir());
    }
    else if(ball->collidesWithItem(lineBottom))
    {
        if(!(ball->getDir()<180))
        {
            qreal angle = 180 + (180 - ball->getDir());
            ball->setDir(angle);
        }
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
