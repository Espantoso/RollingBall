#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent = 0);
    QGraphicsScene *scene;
signals:
public slots:
    void mousePressEvent(QMouseEvent * e);
    // void mouseReleaseEvent(QMouseEvent * e);
    // void mouseDoubleClickEvent(QMouseEvent * e);
    // void mouseMoveEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *e);
};
#endif // MYQGRAPHICSVIEW_H
