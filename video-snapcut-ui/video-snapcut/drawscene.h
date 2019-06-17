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
#include "../tester/sharedmatting.h"

class DrawScene : public QGraphicsScene
{
public:
    DrawScene(QObject *parent = nullptr);
    void setImgSize(int x, int y);

    QGraphicsPixmapItem* getPixmapItem();
    void setImage(QImage *_img);
    void setFgdRadius(int _r);
    void setBgdRadius(int _r);
    void setOutlineRadius(int _r);

    void showImageFromDisk(std::string path);
    void showImageFromOpenCV(cv::Mat& cvImage);
    void showMagnify();
    void unshowMagnify();

    void setRunner(Run *_runner);
    void setCurFrame(Frame *_frame);
    void showCurFrame();

    void clearUserPath();
    void clearOutline();

    enum ShowMode {
        Origin, Cut, Seg
    };

public slots:
    void setCurrentShape(Shape::Code s) {
        std::cout << " [set]: now set shape mode to: " << s << std::endl;
        curShapeCode = s;
    }
    void setCurrentShowMode(ShowMode s) {
        std::cout << " [set]: now set show mode to: " << s << std::endl;
        curShowMode = s;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    ShowMode curShowMode = ShowMode::Origin;
    Shape::Code curShapeCode = Shape::Code::Foreground;
    Shape *curItem = nullptr;

    QGraphicsPathItem *lastPath = nullptr;
    std::list<QGraphicsPathItem *> curPath;

    QGraphicsPixmapItem *pixmapItem = nullptr;
    QImage *img = nullptr;
    int imgX, imgY;

    QGraphicsRectItem *magnify = nullptr;
    QGraphicsRectItem colors[9][9];

    int fgdRadius = 2;
    int bgdRadius = 2;
    int outLineRadius = 6;

    std::list<MyLine*> outlines;
    std::list<QGraphicsPathItem *> outlinesPaths;

    Run* runner = nullptr;
    Frame* curFrame = nullptr;

    void maskGenerator(QPainterPath& maskPath);
    void usermaskGenerator();
    void trimapGenerator();
    QImage matToQImage(const cv::Mat& mat);

    QColor fgdColor, bgdColor, outlineColor;
};

#endif // DRAWSCENE_H
