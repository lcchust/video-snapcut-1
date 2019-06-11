#include "drawpanel.h"

DrawPanel::DrawPanel(QWidget *parent) : QGraphicsView(parent)
{
    uchar *buf = new uchar[width() * height() * 3];
    showingImage = new QImage(buf, width(), height(), QImage::Format_RGB888);
}

void DrawPanel::mousePressEvent(QMouseEvent *)
{

}

void DrawPanel::mouseMoveEvent(QMouseEvent *)
{

}

void DrawPanel::mouseReleaseEvent(QMouseEvent *)
{

}

void DrawPanel::paintEvent(QPaintEvent *event)
{
    QPainter p;
    p.begin(this);
    p.drawImage(0, 0, *showingImage);
    p.end();
    QWidget::paintEvent(event);
}

void DrawPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    drawBuffer();
}


void DrawPanel::setImage(cv::Mat *image)
{
    curImage = image;
    if (showingImage == nullptr)
        return;

    std::cout << "hehe" << std::endl;
    cv::Mat dest;
    cv::resize(*image, dest, cv::Size(width(), height()));
    memcpy(showingImage->bits(), dest.data, dest.cols * dest.rows * dest.elemSize());
}

void DrawPanel::drawBuffer()
{

}
