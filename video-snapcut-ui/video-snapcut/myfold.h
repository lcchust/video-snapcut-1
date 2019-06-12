#ifndef MYFOLD_H
#define MYFOLD_H

#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include "shape.h"

class MyFold: public Shape
{
public:
    MyFold();

    void startDraw(QGraphicsSceneMouseEvent *event);
    void drawing(QGraphicsSceneMouseEvent *event);
    QPainterPath& getPath();

private:

};

#endif // MYFOLD_H
