#ifndef MYLINE_H
#define MYLINE_H

#include "shape.h"
#include <QPainterPath>

class MyLine: public Shape
{
public:
    MyLine();

    void startDraw(QGraphicsSceneMouseEvent *event);
    void drawing(QGraphicsSceneMouseEvent *event);
    QPainterPath& getPath();
};

#endif // MYLINE_H
