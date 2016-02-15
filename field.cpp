#include "field.h"
Field::Field(QString mas[WIDTH][HEIGHT], int width, int height)
{
    for(int i=0; i<width; i++)
        for(int j=0; j<height; j++)
            field[i][j].type=mas[i][j];
    width_in_cells=width;
    height_in_cells=height;
}
void Field::draw(float width, float height, QGraphicsScene *scene)
{
    float optimal_height=height/height_in_cells;
    float optimal_width=width/width_in_cells;
    float size=optimal_height>optimal_width?optimal_width:optimal_height;
    for(int i=0; i<width_in_cells; i++)
        for(int j=0; j<height_in_cells; j++)
        {
            if(field[i][j].type=="abyss")
                scene->addRect(i*size, j*size, size, size, QPen(Qt::black), QBrush(Qt::black));
            else if(field[i][j].type=="empty")
                scene->addRect(i*size, j*size, size, size, QPen(Qt::white), QBrush(Qt::white));
        }
}
