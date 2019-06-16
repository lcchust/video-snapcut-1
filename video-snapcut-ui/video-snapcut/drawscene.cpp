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

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            colors[i][j].setPen(QPen(Qt::gray, 1));
            colors[i][j].setRect(0, 0, 15, 15);
            colors[i][j].setZValue(1);
            colors[i][j].setFlags(QGraphicsItem::ItemIsMovable);
            //colors[i][j].show();
            this->addItem(&colors[i][j]);
        }
    }
    colors[4][4].setPen(QPen(Qt::red, 1));
    unshowMagnify();

    pixmapItem = new QGraphicsPixmapItem;
    pixmapItem->setZValue(0);
    this->addItem(pixmapItem);
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (curFrame == nullptr)
        return;

    std::cout << curFrame << std::endl;

    switch(curShapeCode) {
        case Shape::Foreground: {
            MyLine *line = new MyLine;
            std::cout << "before push" << std::endl;
            curFrame->addfgd(line);
            std::cout << "after push" << std::endl;
            line->setPen(QPen(Qt::white, fgdRadius));
            curItem = line;
            break;
        }
        case Shape::Background: {
            MyLine *line = new MyLine;
            curFrame->bgdList.push_back(line);
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

    if (curShapeCode != Shape::None) {
        int sx = event->scenePos().x();
        int sy = event->scenePos().y();

        if (sx < 0 || sy < 0 || sx >= imgX || sy >= imgY) {
            unshowMagnify();
        } else {
            showMagnify();
            magnify->setPos(sx - 200, sy - 200);

            if (img != nullptr) {
                int x = sx - 4;
                int y = sy - 4;
                sx -= 200;
                sy -= 200;
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if (x < 0 || y < 0 || x >= imgX || y >= imgY)
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
    if (curFrame == nullptr) {
        std::cout << "No image!" << std::endl;
        return;
    }

    QImage mask(imgX, imgY, QImage::Format_RGB888);
    QPainter p(&mask);

    p.setPen(QPen(Qt::gray));
    p.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    p.drawRect(0, 0, imgX, imgY);

    for (MyLine *line: curFrame->fgdList) {
        p.setPen(line->getPen());
        line->draw(&p);
    }

    for (MyLine *line: curFrame->bgdList) {
        p.setPen(line->getPen());
        line->draw(&p);
    }

    p.end();

    bool saved = mask.save("/Users/97littleleaf11/Desktop/haha.png", "png");
    std::cout << saved << std::endl;

    cv::Mat tmp = cv::Mat(mask.height(), mask.width(), CV_8UC3, (void*)mask.constBits(), mask.bytesPerLine());
    cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
    curFrame->update_user_mask(tmp);
    if (runner != nullptr)
        runner->redo();
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
    if (img != nullptr)
        delete img;
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

void DrawScene::showMagnify()
{
    magnify->show();
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            colors[i][j].show();
}

void DrawScene::unshowMagnify()
{
    magnify->hide();
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            colors[i][j].hide();
}

void DrawScene::showImageFromDisk(std::string path)
{
    std::cout << "path: " << path << std::endl;
    QImage* img = new QImage();
    img->load(QString(path.c_str()));
    getPixmapItem()->setPixmap(QPixmap::fromImage(*img));
    setImage(img);
    setImgSize(img->rect().x(), img->rect().y());
    std::cout << img->rect().x() << " " << img->rect().y() << std::endl;
}

void DrawScene::showImageFromOpenCV(cv::Mat& cvImage)
{
    QImage *img = new QImage(matToQImage(cvImage));
    getPixmapItem()->setPixmap(QPixmap::fromImage(*img));
    setImage(img);
    setImgSize(cvImage.cols, cvImage.rows);
}

void DrawScene::setRunner(Run *_runner)
{
    runner = _runner;
}

void DrawScene::setCurFrame(Frame *_frame)
{
    curFrame = _frame;
}

QImage DrawScene::matToQImage(const cv::Mat& mat)
{
    if (mat.type() == CV_8UC1) {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++) {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++) {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    else if (mat.type() == CV_8UC3) {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4) {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void DrawScene::showCurFrame()
{
    if (curFrame == nullptr)
        return;

//    showImageFromDisk(runner->getCurFramePath());

//    std::cout << CV_8UC3 << std::endl;
//    std::cout << curFrame->get_frame().type() << std::endl;

//    QImage *img = curFrame->getQImage();
//    getPixmapItem()->setPixmap(QPixmap::fromImage(*img));
//    setImage(img);
//    setImgSize(img->rect().x(), img->rect().y());

    showImageFromOpenCV(curFrame->get_frame());
}
