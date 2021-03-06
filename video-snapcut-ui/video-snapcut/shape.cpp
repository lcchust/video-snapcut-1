#include "shape.h"

Shape::Shape()
{
}

QPainterPath& Shape::getPath()
{
    return path;
}

std::list<QPoint>& Shape::getPoints()
{
    return points;
}

void Shape::setPen(QPen _pen)
{
    pen = _pen;
}

QPen Shape::getPen()
{
    return pen;
}

void Shape::setShowPen(QPen _pen)
{
    showpen = _pen;
}

QPen Shape::getShowPen()
{
    return showpen;
}

