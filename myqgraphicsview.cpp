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
#include <QSemaphore>
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
bool isTimeLaunched, AddMode, isInChooseLevelMenu=false, isLevelListLoaded=false;
QVector <QByteArray> level_names;
int levels_number_on_inset, inset_number, cur_inset=1;
class LevelListCreator:public QThread
{
public:
    void run()
    {
        int i=1;
        while(true)
        {
            QFile file(QString("levels/level")+QString::number(i)+QString(".txt"));
            if((file.open(QIODevice::ReadOnly))&&(i<100))
            {
                QByteArray level_name=file.readLine();
                //почему-то иногда название уровня не считывается
                //для исправления этого нужен этот цикл с кодом первой буквы названия уровня
                while(level_name.data()[0]!=108)//108 - код буквы l
                    level_name=file.readLine();//прочитаем название уровня ещё раз
                QSemaphore sem(1);
                level_names.append(level_name);
                sem.acquire(1);
                file.close();
            }
            else
            {
                isLevelListLoaded=true;
                break;
            }
            i++;
        }
    }
};
/*class LevelListDrawer:public QThread
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
        QGraphicsTextItem *LevelName=new QGraphicsTextItem("212");
        //QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_names.at(i));
        //LevelName->setPlainText("212");
        while(isInChooseLevelMenu)
        {
            int length=level_names.length();
            for(int i=start; i<length; i++)
            {
                if(i%2==0)
                {
                    scene->addRect(0, 50*i, 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                    std::cout<<i;
                    std::cout<<"\n";
                    std::cout.flush();
                    //LevelName->setPlainText("S");
                    //delete LevelName;
                    //LevelName->setFont(QFont("helvetica", 20));
                    //LevelName->setPos(0, 50*i);
                    //LevelName->setTextWidth(200);
                    //LevelName->document()->setPageSize(QSizeF(200, 50));
                    //LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                    //scene->addItem(LevelName);
                    //delete LevelName;
                }
                else
                {
                    scene->addRect(300, 50*(i-1), 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                    /*QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_names.at(i));
                    LevelName->setFont(QFont("helvetica", 20));
                    LevelName->setPos(300, 50*(i-1));
                    LevelName->setTextWidth(200);
                    LevelName->document()->setPageSize(QSizeF(300, 50));
                    LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                    scene->addItem(LevelName);
                    delete LevelName;
                }
            }
            start=length;
        }
    }
};*/
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
void MyQGraphicsView::StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView, QFile *file)
{
    AddMode=true;
    isInMenu=false;
    delete scene;
    scene = new QGraphicsScene;
    this->scene=scene;
    scene->setSceneRect(0, 0, 500, 500);
    graphicsView->setScene(scene);
    if(file->open(QIODevice::ReadOnly))//вывод настроек уровня на экран для тестирования
    {
        QByteArray level_properties=file->readAll();
        QGraphicsTextItem *Levelprp=new QGraphicsTextItem(level_properties);
        Levelprp->setFont(QFont("helvetica", 12));
        Levelprp->setPos(405, 200);
        Levelprp->setTextWidth(300);
        Levelprp->document()->setPageSize(QSizeF(300, 50));
        Levelprp->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
        scene->addItem(Levelprp);
        file->close();
    }
    else
    {
        QGraphicsTextItem *Error=new QGraphicsTextItem("Error: level file was missed");
        Error->setFont(QFont("helvetica", 12));
        Error->setPos(100, 0);
        Error->setTextWidth(300);
        Error->document()->setPageSize(QSizeF(300, 50));
        Error->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
        scene->addItem(Error);
    }
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
    QGraphicsTextItem *Back=new QGraphicsTextItem();
    scene->addRect(-100, 500, 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
    Back->setPlainText("Back");
    Back->setFont(QFont("helvetica", 20));
    Back->setPos(-100, 505);
    Back->setTextWidth(200);
    Back->document()->setPageSize(QSizeF(200, 50));
    Back->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Back);
    int start=0;
    inset_number=0;
    do
    {
        int length=level_names.length();
        if(length<=10)
            levels_number_on_inset=length;
        else
            levels_number_on_inset=10;
        while(inset_number<length/10+1)
        {
            inset_number++;
            QGraphicsTextItem *InsetName=new QGraphicsTextItem();
            InsetName->setPlainText(QString::number(inset_number*10-9)+"-"+QString::number(inset_number*10));
            if(inset_number==1)
                InsetName->setFont(QFont("helvetica", 11, QFont::Bold));
            else
                InsetName->setFont(QFont("helvetica", 10));
            InsetName->setPos(105+(inset_number-1)*50, 505);
            InsetName->setTextWidth(50);
            InsetName->document()->setPageSize(QSizeF(50, 30));
            InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
            scene->addItem(InsetName);
        }
        for(int i=start; i<length; i++)
        {
            if(i>=10)//на вкладке 10 уровней
                break;
            if(i%2==0)//первый столбик
            {
                scene->addRect(0, 50*i, 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                QGraphicsTextItem *LevelName=new QGraphicsTextItem();
                LevelName->setPlainText(level_names.at(i));
                LevelName->setFont(QFont("helvetica", 20));
                LevelName->setPos(0, 3+50*i);
                LevelName->setTextWidth(200);
                LevelName->document()->setPageSize(QSizeF(200, 50));
                LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(LevelName);
            }
            else//второй столбик
            {
                scene->addRect(300, 50*(i-1), 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                QGraphicsTextItem *LevelName=new QGraphicsTextItem();
                LevelName->setPlainText(level_names.at(i));
                LevelName->setFont(QFont("helvetica", 20));
                LevelName->setPos(300, 3+50*(i-1));
                LevelName->setTextWidth(200);
                LevelName->document()->setPageSize(QSizeF(200, 50));
                LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(LevelName);
            }
        }
        start=length;
    }while((isInChooseLevelMenu)&&(!isLevelListLoaded));
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
    for(int i=2; i<=500; i++)
    {
        file=new QFile(QString("levels/level")+QString::number(i)+QString(".txt"));
        file->open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(file);
        writeStream << "level "+QString::number(i)+"\n"; // Посылаем строку в поток для записи
        writeStream << "level settings";
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
    else if(isInChooseLevelMenu)
    {
        for(int i=0; i<levels_number_on_inset; i++)//уровни
        {
            if((i%2==0)&&(pt.x()>0)&&(pt.x()<200)&&(pt.y()>50*i)&&(pt.y()<50*i+50))
            {
                isInChooseLevelMenu=false;
                QFile *file=new QFile(QString("levels/level")+QString::number((cur_inset-1)*10+i+1)+QString(".txt"));
                StartGame(scene, this, file);
            }
            if((i%2==1)&&(pt.x()>300)&&(pt.x()<500)&&(pt.y()>50*(i-1))&&(pt.y()<50*(i-1)+50))
            {
                isInChooseLevelMenu=false;
                QFile *file=new QFile(QString("levels/level")+QString::number((cur_inset-1)*10+i+1)+QString(".txt"));
                StartGame(scene, this, file);
            }
        }
        if((pt.x()>-100)&&(pt.x()<100)&&(pt.y()>500)&&(pt.y()<550))//назад
        {
            isInChooseLevelMenu=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
        for(int i=0; i<inset_number; i++)//номера вкладок
        {
            if((pt.x()>105+i*50)&&(pt.x()<105+i*50+50)&&(pt.y()>505)&&(pt.y()<535))
            {
                scene->addRect(0, 0, 500, 490, QPen(Qt::white), QBrush(Qt::white));
                scene->addRect(105+(cur_inset-1)*50, 505, 50, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *InsetName=new QGraphicsTextItem();
                InsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                InsetName->setFont(QFont("helvetica", 10));
                InsetName->setPos(105+(cur_inset-1)*50, 505);
                InsetName->setTextWidth(50);
                InsetName->document()->setPageSize(QSizeF(50, 30));
                InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(InsetName);
                cur_inset=i+1;
                scene->addRect(105+i*50, 505, 50, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *CurInsetName=new QGraphicsTextItem();
                CurInsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                CurInsetName->setFont(QFont("helvetica", 11, QFont::Bold));
                CurInsetName->setPos(105+(cur_inset-1)*50, 505);
                CurInsetName->setTextWidth(50);
                CurInsetName->document()->setPageSize(QSizeF(50, 30));
                CurInsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(CurInsetName);
                int start=10*i;
                do
                {
                    int length=level_names.length();
                    if(length-10*i<=10)
                        levels_number_on_inset=length-10*i;
                    else
                        levels_number_on_inset=10;
                    for(int j=start; j<length; j++)
                    {
                        if(j>=10+10*i)//на вкладке 10 уровней
                            break;
                        if(j%2==0)//первый столбик
                        {
                            scene->addRect(0, 50*(j-10*i), 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                            QGraphicsTextItem *LevelName=new QGraphicsTextItem();
                            LevelName->setPlainText(level_names.at(j));
                            LevelName->setFont(QFont("helvetica", 20));
                            LevelName->setPos(0, 3+50*(j-10*i));
                            LevelName->setTextWidth(200);
                            LevelName->document()->setPageSize(QSizeF(200, 50));
                            LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                            scene->addItem(LevelName);
                        }
                        else//второй столбик
                        {
                            scene->addRect(300, 50*(j-1-10*i), 200, 50, QPen(Qt::gray), QBrush(Qt::gray));
                            QGraphicsTextItem *LevelName=new QGraphicsTextItem();
                            LevelName->setPlainText(level_names.at(j));
                            LevelName->setFont(QFont("helvetica", 20));
                            LevelName->setPos(300, 3+50*(j-1-10*i));
                            LevelName->setTextWidth(200);
                            LevelName->document()->setPageSize(QSizeF(200, 50));
                            LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                            scene->addItem(LevelName);
                        }
                    }
                    start=length;
                }while((isInChooseLevelMenu)&&(!isLevelListLoaded));
            }
        }
    }
    else//уровень
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
