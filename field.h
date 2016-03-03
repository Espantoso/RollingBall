#ifndef FIELD_H
#define FIELD_H
#define WIDTH 100
#define HEIGHT 100
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"

class Field;

class FieldCell
{
friend Field;
private:
    QString type;
};

class Field
{
public:
    Field(QString mas[WIDTH][HEIGHT], int width, int height);
    void draw(float width, float height, QGraphicsScene *scene);
    friend FieldCell;
private:
    FieldCell field[WIDTH][HEIGHT];
    int width_in_cells;
    int height_in_cells;
};

#endif // FIELD_H
