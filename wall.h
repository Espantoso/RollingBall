#ifndef WALL_H
#define WALL_H
#include <QVector>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <QGraphicsScene>
struct WallPoint
{
    QGraphicsEllipseItem *wall_point;
    double x;
    double y;
    WallPoint(QGraphicsEllipseItem *wall_point, double x, double y)
    {
        this->wall_point=wall_point;
        this->x=x;
        this->y=y;
    }
};
class Wall
{
private:
    QVector<WallPoint *> wall;
    double width;
    Qt::GlobalColor color;
public:
    Wall(double width, Qt::GlobalColor color);
    void addWallPoint(QGraphicsScene *scene, double x, double y);
    int getWallLength();//возвращает длину вектора wall
    QGraphicsEllipseItem *getWallPoint(int i);
    QGraphicsEllipseItem *getWallPoint(double x, double y);
    void clear(QGraphicsScene *scene);//удалить все элементы вектора wall
    void deletePoint(QGraphicsScene *scene, int i);
    void deletePoint(QGraphicsScene *scene, double x, double y);
    int Find(double x, double y);
};
#endif // WALL_H
