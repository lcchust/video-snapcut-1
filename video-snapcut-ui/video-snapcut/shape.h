#ifndef SHAPE_H
#define SHAPE_H

#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPainter>
#include <QPoint>

class Shape
{
public:
    enum Code {
        None,
        Fold,
        Foreground,
        Background,
        Outline
    };

    Shape();

    virtual void startDraw(QGraphicsSceneMouseEvent *event) = 0;
    virtual void drawing(QGraphicsSceneMouseEvent *event) = 0;
    virtual void endDraw(QGraphicsSceneMouseEvent *event) = 0;
    QPainterPath& getPath();
    std::list<QPoint>& getPoints();
    void setPen(QPen _pen);
    void setShowPen(QPen _pen);
    QPen getPen();
    QPen getShowPen();

protected:
    QPen pen;
    QPen showpen;
    QPainterPath path;
    std::list<QPoint> points;
};

#endif // SHAPE_H
