#ifndef DRAWPANEL_H
#define DRAWPANEL_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include <iostream>
#include <opencv2/opencv.hpp>

class DrawPanel : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DrawPanel(QWidget *parent = nullptr);

    void mousePressEvent (QMouseEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event = 0);
    void resizeEvent(QResizeEvent *event = 0);

    void drawBuffer();

    void setImage(cv::Mat *image);

signals:

public slots:

private:
    cv::Mat *curImage;
    QImage *showingImage;
};

#endif // DRAWPANEL_H
