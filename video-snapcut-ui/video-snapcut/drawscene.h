#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "shape.h"
#include "myline.h"
#include "myfold.h"

class DrawScene : public QGraphicsScene
{
public:
    DrawScene(QObject *parent = nullptr);

public slots:
    void setCurrentShape(Shape::Code s) {
        curShapeCode = s;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Shape::Code curShapeCode = Shape::Code::Fold;
    Shape *curItem = nullptr;
    QGraphicsPathItem *lastPath = nullptr;
};

#endif // DRAWSCENE_H
