#include "wall.h"
Wall::Wall(double width, Qt::GlobalColor color)
{
    this->width=width;
    this->color=color;
}
void Wall::addWallPoint(QGraphicsScene *scene, double x, double y)
{
    QGraphicsEllipseItem *el=scene->addEllipse(x, y, width, width, QPen(color), QBrush(color));
    WallPoint* wall_point=new WallPoint(el, x, y);
    wall.append(wall_point);
}
int Wall::getWallLength()//возвращает длину вектора wall
{
    return wall.length();
}
QGraphicsEllipseItem *Wall::getWallPoint(int i)
{
    return wall.at(i)->wall_point;
}
QGraphicsEllipseItem *Wall::getWallPoint(double x, double y)
{
    for(int i=0; i<wall.length(); i++)
    {
        if((wall.at(i)->x==x)&&(wall.at(i)->y==y))
            return wall.at(i)->wall_point;
    }
    return nullptr;
}
void Wall::clear(QGraphicsScene *scene)
{
    for(int i=0; i<wall.length(); i++)
    {
        scene->removeItem(wall.at(i)->wall_point);
    }
    wall.clear();
}
void Wall::deletePoint(QGraphicsScene *scene ,int i)
{
    scene->removeItem(wall.at(i)->wall_point);
    wall.remove(i);
}
void Wall::deletePoint(QGraphicsScene *scene, double x, double y)
{
    for(int i=0; i<wall.length(); i++)
    {
        if((wall.at(i)->x==x)&&(wall.at(i)->y==y))
        {
            scene->removeItem(wall.at(i)->wall_point);
            wall.remove(i);
            return;
        }
    }
}
