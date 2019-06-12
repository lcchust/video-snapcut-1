#include <iostream>
#include "drawscene.h"

DrawScene::DrawScene(QObject *parent):
    QGraphicsScene(parent)
{

}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(curShapeCode) {
        case Shape::Line: {
                MyLine *line = new MyLine;
                curItem = line;
                break;
            }
        case Shape::Fold: {
                if (event->button() == Qt::RightButton) {
                    if (curItem != nullptr)
                        curItem->endDraw(event);
                    if (lastPath != nullptr) {
                        this->removeItem((QGraphicsItem *)lastPath);
                        lastPath = addPath(curItem->getPath(), QPen(Qt::black));
                    }
                    maskGenerator(curItem->getPath());
                    lastPath = nullptr;
                    curItem = nullptr;
                } else {
                    if (curItem == nullptr) {
                        MyFold *fold = new MyFold;
                        curItem = fold;
                    }
                }
                break;
            }
    }
    if (curItem) {
        curItem->startDraw(event);
        if (lastPath != nullptr)
            this->removeItem((QGraphicsItem *)lastPath);
        lastPath = addPath(curItem->getPath(), QPen(Qt::black));
    }
    std::cout << "scene:" << event->scenePos().x() << " " << event->scenePos().y() << std::endl;
    std::cout << "screen:" << event->screenPos().x() << " " << event->screenPos().y() << std::endl;
    QGraphicsScene::mousePressEvent(event);
}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (curItem) {
        curItem->drawing(event);
        if (lastPath != nullptr) {
            this->removeItem((QGraphicsItem *)lastPath);
            lastPath = addPath(curItem->getPath(), QPen(Qt::black));
        }
    }
    std::cout << this->items().size() << std::endl;
    QGraphicsScene::mouseMoveEvent(event);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (curShapeCode == Shape::Line) {
        lastPath = nullptr;
        curItem = nullptr;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void DrawScene::setImgSize(int x, int y)
{
    imgX = x;
    imgY = y;
}

void DrawScene::maskGenerator(QPainterPath& maskPath)
{
    QImage mask(imgX, imgY, QImage::Format_RGB888);
    QPainter p(&mask);

    p.setPen(QPen(Qt::white));
    p.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    p.drawRect(0, 0, imgX, imgY);

    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::black, Qt::SolidPattern));
    p.drawPath(maskPath);

    p.end();

    std::cout << "saving!" << imgX << " " << imgY << std::endl;
    bool saved = mask.save("/Users/97littleleaf11/Desktop/hehe.png", "png");
    std::cout << saved << std::endl;
}
