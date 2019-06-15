#ifndef MYLINE_H
#define MYLINE_H

#include "shape.h"
#include <QPainterPath>
#include <list>

class MyLine: public Shape
{
public:
    MyLine();

    void startDraw(QGraphicsSceneMouseEvent *event);
    void drawing(QGraphicsSceneMouseEvent *event);
    void endDraw(QGraphicsSceneMouseEvent *event);

    void draw(QPainter *p);
};

#endif // MYLINE_H
