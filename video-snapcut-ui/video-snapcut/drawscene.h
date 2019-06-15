#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include "shape.h"
#include "myline.h"
#include "myfold.h"

class DrawScene : public QGraphicsScene
{
public:
    DrawScene(QObject *parent = nullptr);
    void setImgSize(int x, int y);

    QGraphicsPixmapItem* getPixmapItem();
    void setImage(QImage *_img);
    void setFgdRadius(int _r);
    void setBgdRadius(int _r);

public slots:
    void setCurrentShape(Shape::Code s) {
        curShapeCode = s;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Shape::Code curShapeCode = Shape::Code::Foreground;
    Shape *curItem = nullptr;
    QGraphicsPathItem *lastPath = nullptr;

    QGraphicsPixmapItem *pixmapItem = nullptr;
    QImage *img = nullptr;

    QGraphicsRectItem *magnify = nullptr;
    QGraphicsRectItem colors[9][9];

    int imgX, imgY;
    int fgdRadius = 2;
    int bgdRadius = 2;

    std::list<MyLine*> fgdList, bgdList;

    void maskGenerator(QPainterPath& maskPath);
    void usermaskGenerator();
};

#endif // DRAWSCENE_H
