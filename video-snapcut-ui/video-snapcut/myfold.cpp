#include "myfold.h"

MyFold::MyFold()
{

}

void MyFold::startDraw(QGraphicsSceneMouseEvent *event)
{
    if (path.elementCount() == 0)
        path.moveTo(event->scenePos());
    else path.lineTo(event->scenePos());
}

void MyFold::drawing(QGraphicsSceneMouseEvent *event)
{

}
