#include "myline.h"

MyLine::MyLine()
{
}

void MyLine::startDraw(QGraphicsSceneMouseEvent *event)
{
    path.moveTo(event->scenePos());
    points.push_back(event->scenePos().toPoint());
}

void MyLine::drawing(QGraphicsSceneMouseEvent *event)
{
    path.lineTo(event->scenePos());
    points.push_back(event->scenePos().toPoint());
}

void MyLine::endDraw(QGraphicsSceneMouseEvent *event)
{

}

void MyLine::draw(QPainter *p) {
    QPoint last;
    int flag = 0;
    for (QPoint now: points) {
        if (!flag) {
            flag = 1;
        } else {
            p->drawLine(last, now);
        }
        last = now;
    }
}
