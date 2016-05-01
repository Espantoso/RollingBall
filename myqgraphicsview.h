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
private:
    void createWorld(QGraphicsScene *scene, QGraphicsView *graphicsView);
    void createPolygon(int x, int y, int width, int height);
    void createStaticPolygon(int x, int y, int width, int height);
    void createLine(int x1, int y1, int x2, int y2);
    QGraphicsLineItem* createStaticLine(int x1, int y1, int x2, int y2, QString type="not user");
    void createCircle(int x, int y, int r);
	void createCircle(int x, int y, int r, float density, float friction, float restitution, b2Vec2 impulse, b2Vec2 point);
    void deleteLastPlacedObject();

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
    void slot_timerOut();
};
#endif // MYQGRAPHICSVIEW_H
