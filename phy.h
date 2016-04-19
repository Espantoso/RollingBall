#ifndef PHY_H
#define PHY_H

#include <QtGui>
#include <Box2D/Box2D.h>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>

class phy : public QWidget {
    Q_OBJECT

public:
    phy(QWidget *parent = 0);
    ~phy();
private:
    int rInt(int, int);
    QGraphicsScene *scene;
    QGraphicsView *view;
    QTimer *timer;
    b2World *world;
    QList<b2Body*> bodyList;
    QList<b2Body*>::iterator bodyListIt;
    void createPolygon(int x, int y, int width, int height);
    void createStaticPolygon(int x, int y, int width, int height);
    void createLine(int x1, int y1, int x2, int y2);
    void createStaticLine(int x1, int y1, int x2, int y2);
    void createCircle(int x, int y, int r);
    static const int winWidth = 640;
    static const int winHeight = 480;
    static constexpr float32 pixPerUnit = 10.0f;
    static constexpr float32 timeStep = 60.0f;
    static const int32 velocityIterations = 8;
    static const int32 positionIterations = 3;
private slots:
    void updatePhysics();
};

#endif // PHY_H
