#include <iostream>
#include "drawscene.h"

DrawScene::DrawScene(QObject *parent):
    QGraphicsScene(parent)
{
    magnify = new QGraphicsRectItem(0, 0, 135, 135);
    this->addItem(magnify);

    magnify->setPen(QPen(QColor(0, 160, 230)));
    magnify->setZValue(1);
    magnify->setFlags(QGraphicsItem::ItemIsMovable);
    magnify->show();

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            colors[i][j].setPen(QPen(Qt::gray, 1));
            colors[i][j].setRect(0, 0, 15, 15);
            colors[i][j].setZValue(1);
            colors[i][j].setFlags(QGraphicsItem::ItemIsMovable);
            colors[i][j].show();
            this->addItem(&colors[i][j]);
        }
    }
    colors[4][4].setPen(QPen(Qt::red, 1));

    pixmapItem = new QGraphicsPixmapItem;
    pixmapItem->setZValue(0);
    this->addItem(pixmapItem);
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(curShapeCode) {
        case Shape::Foreground: {
            MyLine *line = new MyLine;
            fgdList.push_back(line);
            line->setPen(QPen(Qt::white, fgdRadius));
            curItem = line;
            break;
        }
        case Shape::Background: {
            MyLine *line = new MyLine;
            bgdList.push_back(line);
            line->setPen(QPen(Qt::black, bgdRadius));
            curItem = line;
            break;
        }
        case Shape::Fold: {
            if (event->button() == Qt::RightButton) {
                if (curItem != nullptr)
                    curItem->endDraw(event);
                if (lastPath != nullptr) {
                    this->removeItem((QGraphicsItem *)lastPath);
                    lastPath = addPath(curItem->getPath(), curItem->getPen());
                }
                maskGenerator(curItem->getPath());
                lastPath = nullptr;
                curItem = nullptr;
            } else {
                if (curItem == nullptr) {
                    MyFold *fold = new MyFold;
                    fold->setPen(QPen(Qt::black, 3));
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
        lastPath = addPath(curItem->getPath(), curItem->getPen());
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
            lastPath = addPath(curItem->getPath(), curItem->getPen());
        }
    }
    std::cout << this->items().size() << std::endl;

    int sx = event->scenePos().x();
    int sy = event->scenePos().y();
    magnify->setPos(sx - 200, sy - 200);
    //magnify->setBrush(QBrush(img->pixel(sx, sy)));

    if (img != nullptr) {
        int x = sx - 4;
        int y = sy - 4;
        sx -= 200;
        sy -= 200;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (x < 0 || y < 0 || x > imgX || y > imgY)
                    colors[i][j].setBrush(QBrush(Qt::black));
                else
                    colors[i][j].setBrush(QBrush(img->pixel(x, y)));
                colors[i][j].setPos(sx, sy);
                x += 1;
                sx += 15;
            }
            sx -= 135;
            sy += 15;
            x -= 9;
            y += 1;
        }
    }

    this->update();
    QGraphicsScene::mouseMoveEvent(event);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    switch (curShapeCode) {
//        case Shape::Foreground: {
//            lastPath = nullptr;
//            curItem = nullptr;
//            break;
//        }
//        case Shape::Background: {
//            lastPath = nullptr;
//            curItem = nullptr;
//            break;
//        }
//    }

    if (curShapeCode == Shape::Foreground || curShapeCode == Shape::Background) {
        lastPath = nullptr;
        curItem = nullptr;
        usermaskGenerator();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void DrawScene::maskGenerator(QPainterPath& maskPath)
{
    QImage mask(imgX, imgY, QImage::Format_RGB888);
    QPainter p(&mask);

    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::black, Qt::SolidPattern));
    p.drawRect(0, 0, imgX, imgY);

    p.setPen(QPen(Qt::white));
    p.setBrush(QBrush(Qt::white, Qt::SolidPattern));
    p.drawPath(maskPath);

    p.end();

    std::cout << "saving!" << imgX << " " << imgY << std::endl;
    bool saved = mask.save("/Users/97littleleaf11/Desktop/hehe.png", "png");
    std::cout << saved << std::endl;
}

void DrawScene::usermaskGenerator()
{
    QImage mask(imgX, imgY, QImage::Format_RGB888);
    QPainter p(&mask);

    p.setPen(QPen(Qt::gray));
    p.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    p.drawRect(0, 0, imgX, imgY);

    for (MyLine *line: fgdList) {
        p.setPen(line->getPen());
        line->draw(&p);
    }

    for (MyLine *line: bgdList) {
        p.setPen(line->getPen());
        line->draw(&p);
    }

    p.end();

    bool saved = mask.save("/Users/97littleleaf11/Desktop/haha.png", "png");
    std::cout << saved << std::endl;
}

void DrawScene::setImgSize(int x, int y)
{
    imgX = x;
    imgY = y;
}

QGraphicsPixmapItem* DrawScene::getPixmapItem()
{
    return pixmapItem;
}

void DrawScene::setImage(QImage *_img)
{
    img = _img;
}

void DrawScene::setFgdRadius(int _r)
{
    fgdRadius = _r;
}

void DrawScene::setBgdRadius(int _r)
{
    bgdRadius = _r;
}
