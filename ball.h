#ifndef BALL_H
#define BALL_H
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"

enum directions {N, S, W, E};

class Ball
{
public:
	Ball(directions dir, double velicity);

private:
	directions dir;
	double velocity;


};

#endif // BALL_H
