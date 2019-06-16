#ifndef DRAWSCENE_H
#define DRAWSCENE_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

#include <opencv2/opencv.hpp>

#include "shape.h"
#include "myline.h"
#include "myfold.h"
#include "../../src/frame.hpp"
#include "../../src/run.hpp"

class DrawScene : public QGraphicsScene
{
public:
    DrawScene(QObject *parent = nullptr);
    void setImgSize(int x, int y);

    QGraphicsPixmapItem* getPixmapItem();
    void setImage(QImage *_img);
    void setFgdRadius(int _r);
    void setBgdRadius(int _r);

    void showImageFromDisk(std::string path);
    void showImageFromOpenCV(cv::Mat& cvImage);
    void showMagnify();
    void unshowMagnify();

    void setRunner(Run *_runner);
    void setCurFrame(Frame *_frame);
    void showCurFrame();

public slots:
    void setCurrentShape(Shape::Code s) {
        std::cout << " [set]: now set mode to: " << s << std::endl;
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
    int imgX, imgY;

    QGraphicsRectItem *magnify = nullptr;
    QGraphicsRectItem colors[9][9];

    int fgdRadius = 2;
    int bgdRadius = 2;

    Run* runner = nullptr;
    Frame* curFrame = nullptr;

    void maskGenerator(QPainterPath& maskPath);
    void usermaskGenerator();
    QImage matToQImage(const cv::Mat& mat);
};

#endif // DRAWSCENE_H
