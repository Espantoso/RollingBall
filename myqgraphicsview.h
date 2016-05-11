#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <stack>
#include <QtGui>
#include <Box2D/Box2D.h>
#include <QMainWindow>

class dynamicBodyDefaultParams
{
public:
    dynamicBodyDefaultParams(float32 defAngle, QPointF defA, QPointF defB, b2Body* defBody)
    {
        this->angle = defAngle;
        this->a = defA;
        this->b = defB;
        this->body = defBody;
    }
    dynamicBodyDefaultParams(QPointF defA, QPoint defB, b2Body* defBody)
    {
        this->a = defA;
        this->b = defB;
        this->body = defBody;
    }
    dynamicBodyDefaultParams(float32 defAngle, QPointF defA, b2Body* defBody)
    {
        this->angle = defAngle;
        this->a = defA;
        this->body = defBody;
    }
private:
    float32 angle;
    QPointF a;
    QPointF b;
    b2Body* body;
};

class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent = 0);
    QGraphicsScene *scene;
    void StartGame(QGraphicsScene *scene, QGraphicsView *graphicsView, QFile *file);
    void ChooseLevel(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void BackToGame(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void Completed();
    void LaunchTime(QGraphicsScene *scene);
    void RestartGame(QGraphicsScene *scene, QGraphicsView *graphicsView);
	void RecordsTable(QGraphicsScene *scene, QGraphicsView *graphicsView);
	void LevelEditor(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void UpdateScore();
    void MainMenu(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void SelectAddWallTool(QGraphicsScene *scene);
    void SelectRemoveWallTool(QGraphicsScene *scene);
    void PauseMenu(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void DrawButton(QGraphicsScene *scene, qreal x, qreal y, qreal w, qreal h, const char *Text);
    static bool isLevelListLoaded;
    static QVector <QString> level_records;
    static QVector <QByteArray> level_names;
private:
    void createWorld(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void createPolygon(int x, int y, int width, int height);
    void createStaticPolygon(int x, int y, int width, int height);
    void createLine(int x1, int y1, int x2, int y2);
    QGraphicsLineItem* createStaticLine(int x1, int y1, int x2, int y2, QString type="not user");
    void createCircle(int x, int y, int r);
	void createCircle(int x, int y, int r, float density, float friction, float restitution, b2Vec2 impulse, b2Vec2 point);
    void deleteLastPlacedObject();
    QGraphicsLineItem *lineLeft;
    QGraphicsLineItem *lineRight;
    QGraphicsLineItem *lineTop;
    QGraphicsLineItem *lineBottom;
    QGraphicsRectItem *finish;
    QGraphicsScene *GameScene;
    QTimer *timer=nullptr;
    qreal finish_x=250, finish_y=250, finish_w=50, finish_h=50;
    const float WallWidth=5, DistBetweenWallEl=3, BallR=20;
    float PrevPointX=-100, PrevPointY;
    QPointF BallStartPos=QPointF(100, 100);
    float BallStartDir=270+45;
    bool isInMenu, isInMainMenu, isInPauseMenu, isInCompletedMenu=false, LeftButtonDown=false;
    bool isTimeLaunched, AddMode, isInChooseLevelMenu=false, isInLevelEditor=false;
    int levels_number_on_inset, inset_number, cur_inset=1, cur_level;
    double score=1000;
    QGraphicsTextItem *Score=nullptr;
    bool isZeroScore=false, isInRecordsTable=false;
    static const int winWidth = 1024;
    static const int winHeight = 768;
    static const int rightOffset = 500, bottomOffset = 100, leftOffset = 20, topOffset = 20;
    static constexpr float32 pixPerUnit = 10.0f;
    static constexpr float32 timeStep = 60.0f;
    static const int32 velocityIterations = 8;
    static const int32 positionIterations = 3;

    //QTimer *timer;
    b2World *world;
    QList<b2Body*> bodyList;
    QList<b2Body*>::iterator bodyListIt;

    std::stack<QPointF> coordsStack;
    QGraphicsLineItem *tmpLine = new QGraphicsLineItem;

    //std::stack<b2Body*> userObjectsStack;
    //std::stack<QGraphicsItem*> userGraphObjectsStack;
    QVector <b2Body*> userObjects;
    QList<dynamicBodyDefaultParams> defParams;
    QList<dynamicBodyDefaultParams>::iterator defParamsIt;
    b2Body* ballPointer;
    b2Vec2 ballDefPos;
    //int numOfUserObjects;
    //modes
    bool simulation;

signals:
    void signal_moveToNextPosition();

public slots:
    void updatePhysics();
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    //void mouseDoubleClickEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *e);
};
#endif // MYQGRAPHICSVIEW_H
