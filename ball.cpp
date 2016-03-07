#include "ball.h"

Ball::Ball(QPointF start_pos, double direction)
{
    this->start_pos=start_pos;
	this->dir=direction;
    this->setPos(start_pos);
}
QRectF Ball::boundingRect() const
{
    return QRectF(0, 0, 500, 500);
}
void Ball::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)//отрисовка мяча
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();
    painter->setPen(Qt::transparent);//без контура
    painter->setBrush(QBrush(Qt::red));
    painter->drawEllipse(start_pos, 20, 20);
    painter->restore();
}
QPainterPath Ball::shape() const
{
    QPainterPath path;
    path.addEllipse(start_pos, 20, 20);
    return path;
}
QPointF Ball::getNextPoint(QPointF startPoint, qreal angle, qreal leng)
{
    QPointF point;
    qreal angleRad = ((angle+90) * 3.1415)/180;
    point.setX(startPoint.x() + (leng * sin(angleRad)));
    point.setY(startPoint.y() + (leng * cos(angleRad)));
    //point.setX(startPoint.x() + (cos(angleRad))-start_pos.rx());
    //point.setY(startPoint.y() + (sin(angleRad))-start_pos.ry());
    return point;
}
void Ball::slot_moveToNextPosition()
{
    this->setPos(getNextPoint(start_pos, dir, meliage));
    QPointF a=this->pos();
    meliage++;
}
double Ball::getDir()
{
    return dir;
}
void Ball::setDir(double dir)
{
    this->dir=dir;
    start_pos = this->pos();
    meliage=1;
}
void Ball::setMeliage(double mel)
{
    this->meliage=mel;
}
