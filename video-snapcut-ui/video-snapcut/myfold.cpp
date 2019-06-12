#include "myfold.h"

MyFold::MyFold()
{

}

void MyFold::startDraw(QGraphicsSceneMouseEvent *event)
{
    if (path.elementCount() == 0)
        path.moveTo(event->scenePos());
    else path.lineTo(event->scenePos());
    points.push_back(event->scenePos().toPoint());
}

void MyFold::drawing(QGraphicsSceneMouseEvent *event)
{

}

void MyFold::endDraw(QGraphicsSceneMouseEvent *event)
{
    path.closeSubpath();
}

std::list<QPoint>& MyFold::getPoints()
{
    return points;
}
