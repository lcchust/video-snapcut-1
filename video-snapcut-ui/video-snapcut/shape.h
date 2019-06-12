#ifndef SHAPE_H
#define SHAPE_H

#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

class Shape
{
public:
    enum Code {
        Line,
        Fold
    };

    Shape();

    virtual void startDraw(QGraphicsSceneMouseEvent *event) = 0;
    virtual void drawing(QGraphicsSceneMouseEvent *event) = 0;
    QPainterPath& getPath();

protected:
    QPainterPath path;
};

#endif // SHAPE_H
