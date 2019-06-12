#include "myline.h"

MyLine::MyLine()
{

}

void MyLine::startDraw(QGraphicsSceneMouseEvent *event)
{
    path.moveTo(event->scenePos());
}

void MyLine::drawing(QGraphicsSceneMouseEvent *event)
{
    path.lineTo(event->scenePos());
}

void MyLine::endDraw(QGraphicsSceneMouseEvent *event)
{

}
