#ifndef BALL_H
#define BALL_H
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
class Ball: public QObject, public QGraphicsItem
{
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
    Ball(QPointF start_pos, double dir);
    Ball();
    QPointF getNextPoint(QPointF startPoint, qreal angle, qreal leng);
    double getDir();
    void setDir(double dir);
    void setMeliage(double mel);
    void setStartPosition(QPointF pos);
private:
    double dir;
    double meliage=1;
    QPointF start_pos;
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
public slots:
    void slot_moveToNextPosition();
};

#endif // BALL_H
