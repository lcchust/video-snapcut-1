#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <opencv2/opencv.hpp>

#include "opencvtester.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // drawView
    drawScene = new DrawScene(ui->drawView);
    //drawScene->setSceneRect(ui->drawView->rect());
    pixmapItem.setZValue(0);
    drawScene->addItem(&pixmapItem);
    ui->drawView->setScene(drawScene);
    ui->drawView->setRenderHint(QPainter::Antialiasing);

    // Menu
    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);

    ui->drawView->show();

    OpencvTester b;
    cv::Mat img_1 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/keyframe/keyframe-01.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat img_2 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/keyframe/keyframe-15.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat mask = cv::imread("/Users/97littleleaf11/Desktop/hehe.png", CV_LOAD_IMAGE_GRAYSCALE);
    QTime t;
    t.start();
    b.testORBPoseEstimation(img_1, img_2, mask);
    std::cout << "motion estimation: " << t.elapsed() << "ms" << std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    QFileDialog fileDiolog;
    fileDiolog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDiolog.setViewMode(QFileDialog::Detail);
    fileDiolog.setFileMode(QFileDialog::ExistingFile);
    fileDiolog.setWindowTitle(tr("打开文件"));

    QStringList filters;
    filters << "JPEG(*.jpg *.jpeg *.jpe *.jfif)"
            << "MP4(*.mp4)"
            << "PNG(*.png)"
            << "ALL FILES(*)";
    fileDiolog.setNameFilters(filters);

    if (fileDiolog.exec() == QDialog::Accepted) {
        QString path = fileDiolog.selectedFiles()[0];
        if (path.length()) {
            std::cout << path.toStdString() << std::endl;

            cv::Mat inputImage = cv::imread(path.toStdString());
            cv::cvtColor(inputImage, inputImage, CV_BGR2RGB);
            QImage img = QImage((const unsigned char*)(inputImage.data),
                                inputImage.cols, inputImage.rows,
                                inputImage.cols * inputImage.channels(),
                                QImage::Format_RGB888);
            pixmapItem.setPixmap(QPixmap::fromImage(img));
            drawScene->setImgSize(inputImage.cols, inputImage.rows);
            //ui->drawView->fitInView(&pixmapItem, Qt::KeepAspectRatio);

        } else {
            QMessageBox::information(this, tr("打开文件失败"), tr("打开文件失败!"));
        }
    }
}
