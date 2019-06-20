#ifndef MYFOLD_H
#define MYFOLD_H

#include <QPoint>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

#include "shape.h"
#include <list>

class MyFold: public Shape
{
public:
    MyFold();

    void startDraw(QGraphicsSceneMouseEvent *event);
    void drawing(QGraphicsSceneMouseEvent *event);
    void endDraw(QGraphicsSceneMouseEvent *event);
};

#endif // MYFOLD_H
