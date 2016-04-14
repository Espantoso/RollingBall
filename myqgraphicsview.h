#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#define PLANE_WIDTH 10
#define PLANE_HEIGHT 10

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
signals:
    void signal_moveToNextPosition();
public slots:
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    //void mouseDoubleClickEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *e);
    void slot_timerOut();
};
#endif // MYQGRAPHICSVIEW_H
