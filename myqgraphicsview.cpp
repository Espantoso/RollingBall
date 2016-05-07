#include "myqgraphicsview.h"
#include "mainwindow.h"
#include "field.h"
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
#include <QMessageBox>
bool MyQGraphicsView::isLevelListLoaded=false;
QVector <QString> MyQGraphicsView::level_records;
QVector <QByteArray> MyQGraphicsView::level_names;
class LevelListCreator:public QThread
{
public:
    void run()
    {
        int i=1;
        while(true)
        {
            QFile file(QString("levels/level")+QString::number(i)+QString(".txt"));
            if((file.open(QIODevice::ReadOnly))&&(i<=100))
            {
                QByteArray level_name=file.readLine();
                //почему-то иногда название уровня не считывается
                //для исправления этого нужен этот цикл с кодом первой буквы названия уровня
                while(level_name.data()[0]!=108)//108 - код буквы l
                    level_name=file.readLine();//прочитаем название уровня ещё раз
                QTextStream instream(&file);
                QString record="-";
                while(!instream.atEnd())
                {
                    QString line=instream.readLine();
                    QString begin=line.mid(0,7);
                    if(begin=="Record:")
                    {
                        int i;
                        for(i=7; !(line.at(i).isDigit()); i++);
                        record=line.mid(i);
                    }
                }
                QSemaphore sem(1);
                MyQGraphicsView::level_names.append(level_name);
                MyQGraphicsView::level_records.append(record);
                sem.acquire(1);
                file.close();
            }
            else
            {
                MyQGraphicsView::isLevelListLoaded=true;
                break;
            }
            i++;
        }
    }
};
void MyQGraphicsView::UpdateScore()
{
    if(score<0)
    {
        Score->setPlainText("Score: "+QString::number(0));
        isZeroScore=true;
    }
    else
    {
        Score->setPlainText("Score: "+QString::number(int(score)));
        isZeroScore=false;
    }
}
void MyQGraphicsView::createWorld(QGraphicsScene *scene, QGraphicsView *view)
{
    view->setRenderHint(QPainter::Antialiasing);
    resize(winWidth, winHeight);
    setFixedSize(winWidth, winHeight);
    /////////////////////////////////////
    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetWarmStarting(true);

    //playground
    createStaticLine(leftOffset, topOffset, leftOffset, winHeight-bottomOffset);//left border
    createStaticLine(leftOffset, winHeight-bottomOffset, winWidth-rightOffset, winHeight-bottomOffset);//bottom border
    createStaticLine(winWidth-rightOffset, leftOffset, winWidth-rightOffset, winHeight-bottomOffset);//right border
    createStaticLine(leftOffset, topOffset, winWidth-rightOffset, leftOffset);//top border

    createCircle(50, 50, 10);

    //numOfUserObjects = 0;//число объектов, добавленных игроком

    /////////////////////////////////////
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePhysics()));
}

QGraphicsLineItem* MyQGraphicsView::createStaticLine(int x1, int y1, int x2, int y2, QString type/*="not user"*/)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x1 / pixPerUnit, y1 / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2EdgeShape shape;
    shape.Set(b2Vec2(0, 0), b2Vec2((x2-x1) / pixPerUnit, (y2-y1) / pixPerUnit));
    body->CreateFixture(&shape, 0.0f);
    QGraphicsLineItem *line = new QGraphicsLineItem;
    line->setPos(x1, y1);
    line->setLine(0, 0, x2-x1, y2-y1);
    if(type=="user")
    {
        line->setPen(QPen(Qt::gray, 3));
        userObjects.append(body);
    }
    scene->addItem(line);
    //userGraphObjectsStack.push(line);
    body->SetUserData(line);
    return line;
}
void MyQGraphicsView::createCircle(int x, int y, int r)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.angularDamping = 1.0f;
    bodyDef.position.Set(x / pixPerUnit, y / pixPerUnit);
    b2Body *body = world->CreateBody(&bodyDef);
    b2CircleShape shape;
    shape.m_radius = r / pixPerUnit;
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.restitution = 1.0f; // прыгучесть
    body->CreateFixture(&fixtureDef);
    //
    ballPointer = body;
    ballDefPos = body->GetPosition();
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem;
    ellipse->setPos(x, y);
    ellipse->setRect(-r, -r, r*2 ,r*2);
    scene->addItem(ellipse);
    //userGraphObjectsStack.push(ellipse);
	body->ApplyLinearImpulse(b2Vec2(50, 0), b2Vec2(1, 1), 1);
    body->SetUserData(ellipse);

    bodyList<<body;
}
void MyQGraphicsView::createCircle(int x, int y, int r, float density, float friction, float restitution, b2Vec2 impulse, b2Vec2 point)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = 1.0f;
	bodyDef.position.Set(x / pixPerUnit, y / pixPerUnit);
	b2Body *body = world->CreateBody(&bodyDef);
	b2CircleShape shape;
	shape.m_radius = r / pixPerUnit;
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = density;//1.0f;
	fixtureDef.friction = friction;// 0.0f;
	fixtureDef.restitution = restitution;//1.0f; // прыгучесть
	body->CreateFixture(&fixtureDef);
	//
	ballPointer = body;
	ballDefPos = body->GetPosition();
	//
	QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem;
	ellipse->setPos(x, y);
	ellipse->setRect(-r, -r, r*2 ,r*2);
	scene->addItem(ellipse);
	body->ApplyLinearImpulse(impulse, point, 1);
	body->SetUserData(ellipse);

	bodyList<<body;
}
void MyQGraphicsView::updatePhysics()
{
    std::cout<<"begin";
    std::cout.flush();
    if (!bodyList.isEmpty())
    {
        QGraphicsItem *item;
        QGraphicsRectItem *rect;
        QGraphicsLineItem *line;
        QGraphicsEllipseItem *ellipse;
        b2Body *body;
        world->Step(1.0 / timeStep, velocityIterations, positionIterations);
        bodyListIt = bodyList.begin();
        while (bodyListIt!=bodyList.end())
        {
            body = *bodyListIt++;
            item = reinterpret_cast<QGraphicsItem*>(body->GetUserData());
            std::cout<<"1";
            std::cout.flush();
            std::cout<<item;
            std::cout.flush();
            std::cout<<"Length: "<<bodyList.length()<<"\n";
            std::cout.flush();
            item->type();
            std::cout<<item;
            std::cout.flush();
            switch (item->type())
            {
            case 3:                 std::cout<<"3begin";
                std::cout.flush();rect = reinterpret_cast<QGraphicsRectItem*>(item);
                rect->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                rect->setRotation(body->GetAngle());
                std::cout<<"3end";
                std::cout.flush();
                break;
            case 4: std::cout<<"4begin";
                std::cout.flush();ellipse = reinterpret_cast<QGraphicsEllipseItem*>(item);
                ellipse->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                ellipse->setRotation(body->GetAngle());
                std::cout<<"4end";
                std::cout.flush();
                break;
            case 5: std::cout<<"5begin";
                std::cout.flush();line = reinterpret_cast<QGraphicsLineItem*>(item);
                line->setPos(body->GetPosition().x * pixPerUnit, body->GetPosition().y * pixPerUnit);
                line->setRotation(body->GetAngle());
                std::cout<<"5end";
                std::cout.flush();
                break;
            default:            std::cout<<"6";
                std::cout.flush();
            }
            std::cout<<"7";
            std::cout.flush();
        }
    }
    std::cout<<"end";
    std::cout.flush();
}
void MyQGraphicsView::StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView, QFile *file)
{
    AddMode=true;
    isInMenu=false;
    simulation=false;
    delete scene;
    scene = new QGraphicsScene;
    this->scene=scene;
    //scene->setSceneRect(0, 0, 500, 500);
    graphicsView->setScene(scene);
    tmpLine=new QGraphicsLineItem;
    scene->addItem(tmpLine);
    if(file->open(QIODevice::ReadOnly))//вывод настроек уровня на экран для тестирования
    {
        QTextStream instream(file);
        QString level_name=instream.readLine();
        QString level_properties=level_name;
        QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_name);
        LevelName->setFont(QFont("helvetica", 16));
        LevelName->setPos(-120, -30);
        LevelName->setTextWidth(300);
        LevelName->document()->setPageSize(QSizeF(300, 50));
        LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
        QGraphicsTextItem *Record=new QGraphicsTextItem();
        bool isRecord=false;
        while(!instream.atEnd())
        {
            QString line=instream.readLine();
            level_properties+=line;
            QString begin=line.mid(0,7);
            if(begin=="Record:")
            {
                Record->setPlainText(line);
                isRecord=true;
            }
        }
        if(!isRecord)
            Record->setPlainText("Record: -");
        Record->setFont(QFont("helvetica", 12));
        Record->setPos(558, 201);
        Record->setTextWidth(300);
        Record->document()->setPageSize(QSizeF(300, 50));
        Record->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
        scene->addItem(Record);
        scene->addItem(LevelName);
        while(!instream.atEnd())
        {
            QString line=instream.readLine();
            level_properties+=line;
        }
        QGraphicsTextItem *Levelprp=new QGraphicsTextItem(level_properties);
        Levelprp->setFont(QFont("helvetica", 12));
        Levelprp->setPos(625, 200);
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
    scene->addRect(560, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Start=new QGraphicsTextItem("Start");
    Start->setFont(QFont("helvetica", 20));
    Start->setPos(442, 45);
    Start->setTextWidth(300);
    Start->document()->setPageSize(QSizeF(300, 50));
    Start->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Start);
    scene->addRect(560, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
    scene->addLine(570, 110, 580, 100, QPen(Qt::gray, 3));
    scene->addRect(600, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addLine(610, 110, 620, 100, QPen(Qt::red, 3));
    scene->addLine(610, 100, 620, 110, QPen(Qt::red, 3));
    scene->addRect(560, 140, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *RemoveAll=new QGraphicsTextItem("Remove all");
    RemoveAll->setFont(QFont("helvetica", 12));
    RemoveAll->setPos(455, 141);
    RemoveAll->setTextWidth(300);
    RemoveAll->document()->setPageSize(QSizeF(300, 50));
    RemoveAll->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(RemoveAll);
    Score=new QGraphicsTextItem("Score: "+QString::number(score));
    Score->setFont(QFont("helvetica", 12));
    Score->setPos(558, 181);
    Score->setTextWidth(300);
    Score->document()->setPageSize(QSizeF(300, 50));
    Score->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
    scene->addItem(Score);

    if (world != NULL)
    {
        bodyList.clear();
        userObjects.clear();
        while (!coordsStack.empty())
            coordsStack.pop();
    }
    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetWarmStarting(true);

    //playground
    createStaticLine(leftOffset, topOffset, leftOffset, winHeight-bottomOffset);//left border
    createStaticLine(leftOffset, winHeight-bottomOffset, winWidth-rightOffset, winHeight-bottomOffset);//bottom border
    createStaticLine(winWidth-rightOffset, leftOffset, winWidth-rightOffset, winHeight-bottomOffset);//right border
    createStaticLine(leftOffset, topOffset, winWidth-rightOffset, leftOffset);//top border

	b2Vec2 impulse(70, 0), point(1, 1);
	createCircle(50, 50, 10, 1, 0, 1, impulse, point);

	timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePhysics()));
}
void MyQGraphicsView::LevelEditor(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
	AddMode=true;
	isInMenu=false;
	simulation=false;
	delete scene;
	scene = new QGraphicsScene;
	this->scene=scene;
	//scene->setSceneRect(0, 0, 500, 500);
	graphicsView->setScene(scene);
	tmpLine=new QGraphicsLineItem;
	scene->addItem(tmpLine);
	scene->addRect(560, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
	QGraphicsTextItem *Start=new QGraphicsTextItem("Start");
	Start->setFont(QFont("helvetica", 20));
	Start->setPos(442, 45);
	Start->setTextWidth(300);
	Start->document()->setPageSize(QSizeF(300, 50));
	Start->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
	scene->addItem(Start);
	scene->addRect(560, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
	scene->addLine(570, 110, 580, 100, QPen(Qt::gray, 3));
	scene->addRect(600, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
	scene->addLine(610, 110, 620, 100, QPen(Qt::red, 3));
	scene->addLine(610, 100, 620, 110, QPen(Qt::red, 3));
	scene->addRect(560, 140, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
	QGraphicsTextItem *RemoveAll=new QGraphicsTextItem("Remove all");
	RemoveAll->setFont(QFont("helvetica", 12));
	RemoveAll->setPos(455, 141);
	RemoveAll->setTextWidth(300);
	RemoveAll->document()->setPageSize(QSizeF(300, 50));
	RemoveAll->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
	scene->addItem(RemoveAll);
	/*Score=new QGraphicsTextItem("Score: "+QString::number(score));
	Score->setFont(QFont("helvetica", 12));
	Score->setPos(558, 181);
	Score->setTextWidth(300);
	Score->document()->setPageSize(QSizeF(300, 50));
	Score->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
	scene->addItem(Score);*/
	if (world != NULL)
	{
		bodyList.clear();
		userObjects.clear();
		while (!coordsStack.empty())
			coordsStack.pop();
	}
	b2Vec2 gravity(0.0f, 0.0f);
	world = new b2World(gravity);
	world->SetAllowSleeping(true);
	world->SetWarmStarting(true);

	//playground
	createStaticLine(leftOffset, topOffset, leftOffset, winHeight-bottomOffset);//left border
	createStaticLine(leftOffset, winHeight-bottomOffset, winWidth-rightOffset, winHeight-bottomOffset);//bottom border
	createStaticLine(winWidth-rightOffset, leftOffset, winWidth-rightOffset, winHeight-bottomOffset);//right border
	createStaticLine(leftOffset, topOffset, winWidth-rightOffset, leftOffset);//top border

	b2Vec2 impulse(200, 0), point(1, 1);
	createCircle(50, 50, 10, 1, 0, 1, impulse, point);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updatePhysics()));
}

void MyQGraphicsView::ChooseLevel(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInChooseLevelMenu=true;
    isInMenu=true;
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
        while(inset_number<length/10)
        {
            inset_number++;
            QGraphicsTextItem *InsetName=new QGraphicsTextItem();
            InsetName->setPlainText(QString::number(inset_number*10-9)+"-"+QString::number(inset_number*10));
            if(inset_number==1)
                InsetName->setFont(QFont("helvetica", 11, QFont::Bold));
            else
                InsetName->setFont(QFont("helvetica", 10));
            InsetName->setPos(105+(inset_number-1)*50, 505);
            InsetName->setTextWidth(70);
            InsetName->document()->setPageSize(QSizeF(70, 30));
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
void MyQGraphicsView::RestartGame(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    AddMode=true;
    isInMenu=false;
    isTimeLaunched=false;
    QFile *file=new QFile(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
    if(file->open(QIODevice::ReadOnly))//вывод настроек уровня на экран для тестирования
    {
        QTextStream instream(file);
        QString level_name=instream.readLine();
        QString level_properties=level_name;
        QGraphicsTextItem *Record=new QGraphicsTextItem();
        bool isRecord=false;
        while(!instream.atEnd())
        {
            QString line=instream.readLine();
            level_properties+=line;
            QString begin=line.mid(0,7);
            if(begin=="Record:")
            {
                Record->setPlainText(line);
                isRecord=true;
            }
        }
        if(!isRecord)
            Record->setPlainText("Record: -");
        Record->setFont(QFont("helvetica", 12));
        Record->setPos(558, 201);
        Record->setTextWidth(300);
        Record->document()->setPageSize(QSizeF(300, 50));
        Record->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
        scene->addItem(Record);
        QGraphicsTextItem *Levelprp=new QGraphicsTextItem(level_properties);
        Levelprp->setFont(QFont("helvetica", 12));
        Levelprp->setPos(625, 200);
        Levelprp->setTextWidth(300);
        Levelprp->document()->setPageSize(QSizeF(300, 50));
        Levelprp->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
        scene->addItem(Levelprp);
        file->close();
    }
    timer->stop();
    simulation = false;
    ballPointer->SetTransform(ballDefPos, 1.0f);
    ballPointer->SetAngularVelocity(0);
    ballPointer->SetLinearVelocity(b2Vec2(0,0));
    ballPointer->ApplyLinearImpulse(b2Vec2(50, 0), b2Vec2(1, 1), 1);
    updatePhysics();
    scene->addRect(560, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Start=new QGraphicsTextItem("Start");
    Start->setFont(QFont("helvetica", 20));
    Start->setPos(442, 45);
    Start->setTextWidth(300);
    Start->document()->setPageSize(QSizeF(300, 50));
    Start->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Start);
    scene->addRect(560, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
    scene->addRect(560, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
    scene->addLine(570, 110, 580, 100, QPen(Qt::gray, 3));
    scene->addRect(600, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addLine(610, 110, 620, 100, QPen(Qt::red, 3));
    scene->addLine(610, 100, 620, 110, QPen(Qt::red, 3));
    scene->addRect(560, 140, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *RemoveAll=new QGraphicsTextItem("Remove all");
    RemoveAll->setFont(QFont("helvetica", 12));
    RemoveAll->setPos(455, 141);
    RemoveAll->setTextWidth(300);
    RemoveAll->document()->setPageSize(QSizeF(300, 50));
    RemoveAll->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(RemoveAll);
    Score=new QGraphicsTextItem("Score: "+QString::number(int(score)));
    Score->setFont(QFont("helvetica", 12));
    Score->setPos(558, 181);
    Score->setTextWidth(300);
    Score->document()->setPageSize(QSizeF(300, 50));
    Score->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
    scene->addItem(Score);
}
void MyQGraphicsView::MainMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isTimeLaunched=false;
    delete scene;
    scene = new QGraphicsScene;
    graphicsView->setScene(scene);
    QPixmap *image=new QPixmap("images/MainMenu.jpg");
    QGraphicsPixmapItem *ImageItem=scene->addPixmap(*image);
    ImageItem->setPos(-image->width()/2+150, -image->height()/2);

    scene->addRect(0, -150, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *StartGame=new QGraphicsTextItem("Choose level");
    StartGame->setFont(QFont("helvetica", 20));
    StartGame->setPos(0, -150);
    StartGame->setTextWidth(300);
    StartGame->document()->setPageSize(QSizeF(300, 50));
    StartGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(StartGame);
    scene->addRect(0, -50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *RecordsTable=new QGraphicsTextItem("Records table");
    RecordsTable->setFont(QFont("helvetica", 20));
    RecordsTable->setPos(0, -50);
    RecordsTable->setTextWidth(300);
    RecordsTable->document()->setPageSize(QSizeF(300, 50));
    RecordsTable->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(RecordsTable);
    scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Quit=new QGraphicsTextItem("Exit");
    Quit->setFont(QFont("helvetica", 20));
    Quit->setPos(0, 50);
    Quit->setTextWidth(300);
    Quit->document()->setPageSize(QSizeF(300, 50));
    Quit->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Quit);
	scene->addRect(0, 150, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
	QGraphicsTextItem *LevelEditor=new QGraphicsTextItem("Level editor");
	LevelEditor->setFont(QFont("helvetica", 20));
	LevelEditor->setPos(0, 150);
	LevelEditor->setTextWidth(300);
	LevelEditor->document()->setPageSize(QSizeF(300, 50));
	LevelEditor->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
	scene->addItem(LevelEditor);
}
MyQGraphicsView::MyQGraphicsView(QWidget *parent):QGraphicsView(parent)
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
    timer->start(1000 / timeStep);
    isInMenu=false;
    scene = GameScene;
    graphicsView->setScene(scene);
}
void MyQGraphicsView::SelectAddWallTool(QGraphicsScene *scene)
{
    AddMode=true;
    scene->addRect(560, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
    scene->addLine(570, 110, 580, 100, QPen(Qt::gray, 3));
    scene->addRect(600, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addLine(610, 110, 620, 100, QPen(Qt::red, 3));
    scene->addLine(610, 100, 620, 110, QPen(Qt::red, 3));
}
void MyQGraphicsView::SelectRemoveWallTool(QGraphicsScene *scene)
{
    AddMode=false;
    scene->addRect(560, 90, 30, 30, QPen(Qt::black), QBrush(Qt::white));
    scene->addLine(570, 110, 580, 100, QPen(Qt::gray, 3));
    scene->addRect(600, 90, 30, 30, QPen(Qt::red), QBrush(Qt::white));
    scene->addLine(610, 110, 620, 100, QPen(Qt::red, 3));
    scene->addLine(610, 100, 620, 110, QPen(Qt::red, 3));
}
void MyQGraphicsView::LaunchTime(QGraphicsScene *scene)
{
    timer->start(1000 / timeStep);
    simulation = true;
    isTimeLaunched=true;
    scene->addRect(560, 50, 95, 30, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Restart=new QGraphicsTextItem("Restart");
    Restart->setFont(QFont("helvetica", 20));
    Restart->setPos(457, 45);
    Restart->setTextWidth(300);
    Restart->document()->setPageSize(QSizeF(300, 50));
    Restart->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Restart);
    scene->addRect(560, 90, 200, 500, QPen(Qt::white), QBrush(Qt::white));
}
void MyQGraphicsView::RecordsTable(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInRecordsTable=true;
    isInMenu=true;
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
    scene->addRect(0, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
    QGraphicsTextItem *LevelName=new QGraphicsTextItem("level");
    LevelName->setFont(QFont("helvetica", 13, QFont::Bold));
    LevelName->setPos(0, 0);
    LevelName->setTextWidth(250);
    LevelName->document()->setPageSize(QSizeF(250, 50));
    LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(LevelName);
    scene->addRect(250, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
    QGraphicsTextItem *Record=new QGraphicsTextItem("record");
    Record->setFont(QFont("helvetica", 13, QFont::Bold));
    Record->setPos(250, 0);
    Record->setTextWidth(250);
    Record->document()->setPageSize(QSizeF(250, 50));
    Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Record);
    int start=0;
    inset_number=0;
    do
    {
        int length=level_names.length();
        if(length<=10)
            levels_number_on_inset=length;
        else
            levels_number_on_inset=10;
        while(inset_number<length/10)
        {
            inset_number++;
            QGraphicsTextItem *InsetName=new QGraphicsTextItem();
            InsetName->setPlainText(QString::number(inset_number*10-9)+"-"+QString::number(inset_number*10));
            if(inset_number==1)
                InsetName->setFont(QFont("helvetica", 11, QFont::Bold));
            else
                InsetName->setFont(QFont("helvetica", 10));
            InsetName->setPos(105+(inset_number-1)*50, 505);
            InsetName->setTextWidth(70);
            InsetName->document()->setPageSize(QSizeF(70, 30));
            InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
            scene->addItem(InsetName);
        }
        for(int i=start; i<length; i++)
        {
            if(i>=10)//на вкладке 10 уровней
                break;
            scene->addRect(0, 30+30*i, 250, 30, QPen(Qt::black), QBrush(Qt::white));
            QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_names.at(i));
            LevelName->setFont(QFont("helvetica", 13));
            LevelName->setPos(0, 30+30*i);
            LevelName->setTextWidth(250);
            LevelName->document()->setPageSize(QSizeF(250, 50));
            LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
            scene->addItem(LevelName);
            scene->addRect(250, 30+30*i, 250, 30, QPen(Qt::black), QBrush(Qt::white));
            QGraphicsTextItem *Record=new QGraphicsTextItem(level_records.at(i));
            Record->setFont(QFont("helvetica", 13));
            Record->setPos(250, 30+30*i);
            Record->setTextWidth(250);
            Record->document()->setPageSize(QSizeF(250, 50));
            Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
            scene->addItem(Record);
        }
        start=length;
    }while((isInRecordsTable)&&(!isLevelListLoaded));
}
void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    LeftButtonDown=true;
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {
        if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-150)&&(pt.y()<-100))//Начать игру
        {
            isInMainMenu=false;
            ChooseLevel(scene, this);
        }
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>-50)&&(pt.y()<0))//Таблица рекордов
        {
            isInMainMenu=false;
            RecordsTable(scene, this);
        }
		else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>150)&&(pt.y()<200))//Редактор уровней
		{
			isInMainMenu=false;
			LevelEditor(scene, this);
		}
        else if((pt.x()>0)&&(pt.x()<300)&&(pt.y()>50)&&(pt.y()<100))//Выход
            exit(0);
    }
    else if(isInPauseMenu)
    {
        /*if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-125)&&((pt.y()<-75)))//Вернуться к игре
        {
            isInPauseMenu=false;
            BackToGame(scene, this);
        }*/
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>-25)&&((pt.y()<25)))//Главное меню
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
        if((cur_level!=level_names.length())&&(pt.x()>0)&&((pt.x()<300))&&(pt.y()>50)&&((pt.y()<100)))//Следующий уровень
        {
            isInCompletedMenu=false;
            cur_level++;
            QFile *file=new QFile(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
            StartGame(scene, this, file);
        }
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>150)&&((pt.y()<200)))//Рестарт
        {
            isInCompletedMenu=false;
            QFile *file=new QFile(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
            StartGame(scene, this, file);
        }
        if((pt.x()>0)&&((pt.x()<300))&&(pt.y()>250)&&((pt.y()<300)))//Главное меню
        {
            isInCompletedMenu=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
    }
    else if(isTimeLaunched)
    {
        if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>50)&&(pt.y()<80))
        {
            RestartGame(scene, this);
        }
    }
    else if(isInRecordsTable)
    {
        if((pt.x()>-100)&&(pt.x()<100)&&(pt.y()>500)&&(pt.y()<550))//назад
        {
            isInRecordsTable=false;
            isInMainMenu=true;
            MainMenu(scene, this);
        }
        for(int i=0; i<inset_number; i++)//номера вкладок
        {
            if((pt.x()>105+i*50)&&(pt.x()<105+i*50+70)&&(pt.y()>505)&&(pt.y()<535))
            {
                scene->addRect(0, 0, 500, 490, QPen(Qt::white), QBrush(Qt::white));
                scene->addRect(105+(cur_inset-1)*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *InsetName=new QGraphicsTextItem();
                InsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                InsetName->setFont(QFont("helvetica", 10));
                InsetName->setPos(105+(cur_inset-1)*50, 505);
                InsetName->setTextWidth(70);
                InsetName->document()->setPageSize(QSizeF(70, 30));
                InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(InsetName);
                cur_inset=i+1;
                scene->addRect(105+i*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *CurInsetName=new QGraphicsTextItem();
                CurInsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                CurInsetName->setFont(QFont("helvetica", 11, QFont::Bold));
                CurInsetName->setPos(105+(cur_inset-1)*50, 505);
                CurInsetName->setTextWidth(70);
                CurInsetName->document()->setPageSize(QSizeF(70, 30));
                CurInsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(CurInsetName);
                scene->addRect(0, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
                QGraphicsTextItem *LevelName=new QGraphicsTextItem("level");
                LevelName->setFont(QFont("helvetica", 13, QFont::Bold));
                LevelName->setPos(0, 0);
                LevelName->setTextWidth(250);
                LevelName->document()->setPageSize(QSizeF(250, 50));
                LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(LevelName);
                scene->addRect(250, 0, 250, 30, QPen(Qt::black), QBrush(Qt::white));
                QGraphicsTextItem *Record=new QGraphicsTextItem("record");
                Record->setFont(QFont("helvetica", 13, QFont::Bold));
                Record->setPos(250, 0);
                Record->setTextWidth(250);
                Record->document()->setPageSize(QSizeF(250, 50));
                Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(Record);
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
                        scene->addRect(0, 30+30*(j-10*i), 250, 30, QPen(Qt::black), QBrush(Qt::white));
                        QGraphicsTextItem *LevelName=new QGraphicsTextItem(level_names.at(j));
                        LevelName->setFont(QFont("helvetica", 13));
                        LevelName->setPos(0, 30+30*(j-10*i));
                        LevelName->setTextWidth(250);
                        LevelName->document()->setPageSize(QSizeF(250, 50));
                        LevelName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                        scene->addItem(LevelName);
                        scene->addRect(250, 30+30*(j-10*i), 250, 30, QPen(Qt::black), QBrush(Qt::white));
                        QGraphicsTextItem *Record=new QGraphicsTextItem(level_records.at(j));
                        Record->setFont(QFont("helvetica", 13));
                        Record->setPos(250, 30+30*(j-10*i));
                        Record->setTextWidth(250);
                        Record->document()->setPageSize(QSizeF(250, 50));
                        Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                        scene->addItem(Record);
                    }
                    start=length;
                }while((isInChooseLevelMenu)&&(!isLevelListLoaded));
            }
        }
    }
    else if(isInChooseLevelMenu)
    {
        for(int i=0; i<levels_number_on_inset; i++)//уровни
        {
            if((i%2==0)&&(pt.x()>0)&&(pt.x()<200)&&(pt.y()>50*i)&&(pt.y()<50*i+50))
            {
                isInChooseLevelMenu=false;
                cur_level=i+1;
                QFile *file=new QFile(QString("levels/level")+QString::number((cur_inset-1)*10+i+1)+QString(".txt"));
                StartGame(scene, this, file);
            }
            if((i%2==1)&&(pt.x()>300)&&(pt.x()<500)&&(pt.y()>50*(i-1))&&(pt.y()<50*(i-1)+50))
            {
                isInChooseLevelMenu=false;
                cur_level=i+1;
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
            if((pt.x()>105+i*50)&&(pt.x()<105+i*50+70)&&(pt.y()>505)&&(pt.y()<535))
            {
                scene->addRect(0, 0, 500, 490, QPen(Qt::white), QBrush(Qt::white));
                scene->addRect(105+(cur_inset-1)*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *InsetName=new QGraphicsTextItem();
                InsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                InsetName->setFont(QFont("helvetica", 10));
                InsetName->setPos(105+(cur_inset-1)*50, 505);
                InsetName->setTextWidth(70);
                InsetName->document()->setPageSize(QSizeF(70, 30));
                InsetName->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
                scene->addItem(InsetName);
                cur_inset=i+1;
                scene->addRect(105+i*50, 505, 65, 30, QPen(Qt::white), QBrush(Qt::white));
                QGraphicsTextItem *CurInsetName=new QGraphicsTextItem();
                CurInsetName->setPlainText(QString::number(cur_inset*10-9)+"-"+QString::number(cur_inset*10));
                CurInsetName->setFont(QFont("helvetica", 11, QFont::Bold));
                CurInsetName->setPos(105+(cur_inset-1)*50, 505);
                CurInsetName->setTextWidth(70);
                CurInsetName->document()->setPageSize(QSizeF(70, 30));
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
    else//уровень, таймер запущен
    {
        if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>50)&&(pt.y()<80))
        {
            LaunchTime(scene);
            return;
        }
        else if((pt.x()>560)&&(pt.x()<590)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectAddWallTool(scene);
            return;
        }
        else if((pt.x()>600)&&(pt.x()<630)&&(pt.y()>90)&&(pt.y()<120))
        {
            SelectRemoveWallTool(scene);
            return;
        }
        else if((pt.x()>560)&&(pt.x()<655)&&(pt.y()>140)&&(pt.y()<170))//Удалить все стены
        {
            score=1000;
            isZeroScore=false;
            UpdateScore();
            for(int i=0; i<userObjects.length();)
            {
                QGraphicsItem* item=reinterpret_cast<QGraphicsItem*>(userObjects.at(i)->GetUserData());
                world->DestroyBody(userObjects.at(i));
                userObjects.removeAt(i);
                scene->removeItem(item);
            }
            scene->removeItem(tmpLine);
            return;
        }
        if(!simulation)//во время симуляции редактирование запрещено
        {
            if (AddMode&&(pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                if(!isZeroScore)
                {
                    QGraphicsItem *ball=reinterpret_cast<QGraphicsItem*>(ballPointer->GetUserData());
                    QGraphicsEllipseItem *checkCircle=new QGraphicsEllipseItem(pt.x(), pt.y(), 1, 1);
                    if(!ball->collidesWithItem(checkCircle))//если пользователь нажал не на шар
                    {
                        coordsStack.push(pt);
                        if (coordsStack.size()==2)
                        {
                            QPointF temp1 = coordsStack.top();
                            coordsStack.pop();
                            QPointF temp2 = coordsStack.top();
                            QGraphicsLineItem *checkLine=new QGraphicsLineItem(temp1.x(), temp1.y(), temp2.x(), temp2.y());
                            qreal x0=checkLine->boundingRect().x();
                            qreal y0=checkLine->boundingRect().y();
                            qreal x1=checkLine->boundingRect().x()+checkLine->boundingRect().width();
                            qreal y1=checkLine->boundingRect().y()+checkLine->boundingRect().height();
                            qreal length=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                            QGraphicsLineItem *line;
                            if(length<score)
                            {
                                line=createStaticLine(temp1.x(), temp1.y(), temp2.x(), temp2.y(), "user");
                                x0=line->boundingRect().x();
                                y0=line->boundingRect().y();
                                x1=line->boundingRect().x()+line->boundingRect().width();
                                y1=line->boundingRect().y()+line->boundingRect().height();
                                score-=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                                UpdateScore();
                            }
                            else
                            {
                                scene->removeItem(tmpLine);
                                QMessageBox::information(0,"","You can not add line whith such length while score is such small",QMessageBox::Ok|QMessageBox::NoButton);
                            }
                            if (!coordsStack.empty())
                                while (!coordsStack.empty())
                                    coordsStack.pop();
                        }
                    }
                    delete checkCircle;
                }
                else
                    QMessageBox::information(0,"","You can not add lines while score is zero",QMessageBox::Ok|QMessageBox::NoButton);
            }
            if (!AddMode&&(pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                QGraphicsEllipseItem *checkCircle=new QGraphicsEllipseItem(pt.x(), pt.y(), 3, 3);
                for(int i=0; i<userObjects.length(); i++)
                {
                    QGraphicsLineItem* item=reinterpret_cast<QGraphicsLineItem*>(userObjects.at(i)->GetUserData());
                    if(item->collidesWithItem(checkCircle))
                    {
                        qreal x0=item->boundingRect().x();
                        qreal y0=item->boundingRect().y();
                        qreal x1=item->boundingRect().x()+item->boundingRect().width();
                        qreal y1=item->boundingRect().y()+item->boundingRect().height();
                        score+=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                        UpdateScore();
                        world->DestroyBody(userObjects.at(i));
                        userObjects.removeAt(i);
                        scene->removeItem(item);
                        scene->removeItem(tmpLine);
                    }
                }
                delete checkCircle;
            }
        }
    }
}
void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
    LeftButtonDown=false;
}
void MyQGraphicsView::mouseMoveEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    if(isInMainMenu)
    {

    }
    else if(isInPauseMenu)
    {

    }
    else if(isInCompletedMenu)
    {

    }
    else if(isTimeLaunched)
    {

    }
    else if(isInChooseLevelMenu)
    {

    }
    else//уровень
    {
        if(!simulation)
        {
            if ((pt.x()>leftOffset  && pt.y()>topOffset) && (pt.x()<winWidth-rightOffset  && pt.y()<winHeight-bottomOffset))
            {
                if (coordsStack.size()==1)
                {
                    QGraphicsItem *ball=reinterpret_cast<QGraphicsItem*>(ballPointer->GetUserData());
                    QPointF temp1 = coordsStack.top();
                    QGraphicsLineItem *checkLine=new QGraphicsLineItem(temp1.x(), temp1.y(), pt.x(), pt.y());
                    scene->addItem(tmpLine);
                    if(!ball->collidesWithItem(checkLine))//чтобы стена не проходила сквозь шар
                        tmpLine->setLine(temp1.x(), temp1.y(), pt.x(), pt.y());
                    delete checkLine;
                }
            }
        }
    }
}
void MyQGraphicsView::PauseMenu(QGraphicsScene *scene, QGraphicsView *graphicsView)
{
    isInMenu=true;
    GameScene=scene;
    scene = new QGraphicsScene;
    graphicsView->setScene(scene);
    /*scene->addRect(0, -125, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *ReturnToGame=new QGraphicsTextItem("Back to game");
    ReturnToGame->setFont(QFont("helvetica", 20));
    ReturnToGame->setPos(0, -125);
    ReturnToGame->setTextWidth(300);
    ReturnToGame->document()->setPageSize(QSizeF(300, 50));
    ReturnToGame->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(ReturnToGame);*/
    scene->addRect(0, -25, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *MainMenu=new QGraphicsTextItem("Main menu");
    MainMenu->setFont(QFont("helvetica", 20));
    MainMenu->setPos(0, -25);
    MainMenu->setTextWidth(300);
    MainMenu->document()->setPageSize(QSizeF(300, 50));
    MainMenu->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(MainMenu);
    scene->addRect(0, 75, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
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
        userObjects.clear();
        timer->stop();
        isInPauseMenu=true;
        timer->stop();
        simulation = false;
        ballPointer->SetTransform(ballDefPos, 1.0f);
        ballPointer->SetAngularVelocity(0);
        ballPointer->SetLinearVelocity(b2Vec2(0,0));
        ballPointer->ApplyLinearImpulse(b2Vec2(50, 0), b2Vec2(1, 1), 1);
        PauseMenu(scene, this);
    }
    if(isInMainMenu)
    {

    }
    else if(isInPauseMenu)
    {

    }
    else if(isInCompletedMenu)
    {

    }
    else if(isTimeLaunched)
    {

    }
    else if(isInChooseLevelMenu)
    {

    }
    else//уровень
    {
        if (key == Qt::Key_Space)
        {
            LaunchTime(scene);
        }
        if (key == Qt::Key_Backspace)
        {
            if(!simulation)//во время симуляции редактирование запрещено
            {
                if (userObjects.length() > 0)
                {
                    QGraphicsItem* item=reinterpret_cast<QGraphicsItem*>(userObjects.last()->GetUserData());
                    qreal x0=item->boundingRect().x();
                    qreal y0=item->boundingRect().y();
                    qreal x1=item->boundingRect().x()+item->boundingRect().width();
                    qreal y1=item->boundingRect().y()+item->boundingRect().height();
                    score+=qSqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                    UpdateScore();
                    scene->removeItem(item);
                    scene->removeItem(tmpLine);
                    world->DestroyBody(userObjects.last());
                    userObjects.removeLast();
                }
            }
        }
    }
}
void MyQGraphicsView::Completed()
{
    timer->stop();
    isInCompletedMenu=true;
    isInMenu=true;
    isTimeLaunched=false;
    scene = new QGraphicsScene;
    this->setScene(scene);
    QGraphicsTextItem *Completed=new QGraphicsTextItem("Completed!");
    Completed->setFont(QFont("helvetica", 40));
    Completed->setPos(0, -150);
    Completed->setTextWidth(300);
    Completed->document()->setPageSize(QSizeF(300, 100));
    Completed->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Completed);
    if(cur_level!=level_names.length())
    {
        scene->addRect(0, 50, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
        QGraphicsTextItem *NextLevel=new QGraphicsTextItem("Next Level");
        NextLevel->setFont(QFont("helvetica", 20));
        NextLevel->setPos(0, 50);
        NextLevel->setTextWidth(300);
        NextLevel->document()->setPageSize(QSizeF(300, 50));
        NextLevel->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
        scene->addItem(NextLevel);
    }
    scene->addRect(0, 150, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *Restart=new QGraphicsTextItem("Restart");
    Restart->setFont(QFont("helvetica", 20));
    Restart->setPos(0, 150);
    Restart->setTextWidth(300);
    Restart->document()->setPageSize(QSizeF(300, 50));
    Restart->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Restart);
    scene->addRect(0, 250, 300, 50, QPen(Qt::gray), QBrush(Qt::gray));
    QGraphicsTextItem *MainMenu=new QGraphicsTextItem("Main menu");
    MainMenu->setFont(QFont("helvetica", 20));
    MainMenu->setPos(0, 250);
    MainMenu->setTextWidth(300);
    MainMenu->document()->setPageSize(QSizeF(300, 50));
    MainMenu->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(MainMenu);
    QFile input(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
    QFile output("levels/output.txt");
    input.open(QIODevice::ReadOnly);
    output.open(QIODevice::WriteOnly);
    QTextStream instream(&input);
    QTextStream outstream(&output);
    int record=score;
    while(!instream.atEnd())
    {
        QString line=instream.readLine();
        QString begin=line.mid(0,7);
        if(begin=="Record:")
        {
            int i;
            for(i=7; !(line.at(i).isDigit()); i++);
            QString strrecord=line.mid(i);
            record=strrecord.toInt();
            if(score>=record)
                continue;
        }
        outstream<<line+"\r\n";
    }
    if(score>=record)
        outstream<<"Record: "+QString::number(score)+"\r\n";
    input.close();
    output.close();
    input.remove();
    output.rename(QString("levels/level")+QString::number(cur_level)+QString(".txt"));
    QGraphicsTextItem *Score=new QGraphicsTextItem("Score: "+QString::number(score));
    Score->setFont(QFont("helvetica", 12));
    Score->setPos(0, -54);
    Score->setTextWidth(300);
    Score->document()->setPageSize(QSizeF(300, 50));
    Score->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Score);
    QGraphicsTextItem *Record=new QGraphicsTextItem("Record: "+QString::number(record));
    Record->setFont(QFont("helvetica", 12));
    Record->setPos(0, -16);
    Record->setTextWidth(300);
    Record->document()->setPageSize(QSizeF(300, 50));
    Record->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
    scene->addItem(Record);
}
