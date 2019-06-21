#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../tester/opencvtester.h"
#include "../../src/graphcut.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // drawView
    drawScene = new DrawScene(ui->drawView);
    ui->drawView->setScene(drawScene);
    ui->drawView->setMouseTracking(true);
    ui->drawView->setRenderHint(QPainter::Antialiasing);

    // Menu
    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    openDirAction = new QAction(tr("&Open Dir..."), this);
    openDirAction->setShortcut(QKeySequence(tr("Alt+O")));
    openDirAction->setStatusTip(tr("Open a Directory"));
    connect(openDirAction, &QAction::triggered, this, &MainWindow::openDir);

    test1Action = new QAction(tr("&Test Motion Estimation"), this);
    connect(test1Action, &QAction::triggered, this, &MainWindow::test_motion_estimation);
    test2Action = new QAction(tr("&Test GraphCut"), this);
    connect(test2Action, &QAction::triggered, this, &MainWindow::test_graphcut);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);
    file->addAction(openDirAction);
    file->addAction(test1Action);
    file->addAction(test2Action);

    // button icons
    ui->stopButton->setIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)61));
    ui->fwdButton->setIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)66));
    ui->fwdOnceButton->setIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)64));
    ui->bwdButton->setIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)67));
    ui->bwdOnceButton->setIcon(QApplication::style()->standardIcon((enum QStyle::StandardPixmap)65));

    ui->drawView->show();

    //test_alpha_matting();
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
    filters << "images(*.jpg *.jpeg *.jpe *.jfif *.png)"
            << "ALL FILES(*)";
    fileDiolog.setNameFilters(filters);

    if (fileDiolog.exec() == QDialog::Accepted) {
        QString path = fileDiolog.selectedFiles()[0];
        if (path.length()) {
            std::cout << path.toStdString() << std::endl;
            cv::Mat *inputImage = new cv::Mat(cv::imread(path.toStdString()));
            cv::cvtColor(*inputImage, *inputImage, CV_BGR2RGB);
            QImage* img = new QImage((const unsigned char*)(inputImage->data),
                                     inputImage->cols, inputImage->rows,
                                     inputImage->cols * inputImage->channels(),
                                     QImage::Format_RGB888);
            drawScene->getPixmapItem()->setPixmap(QPixmap::fromImage(*img));
            drawScene->setImage(img);
            drawScene->setImgSize(inputImage->cols, inputImage->rows);
            drawScene->showMagnify();
            //ui->drawView->fitInView(&pixmapItem, Qt::KeepAspectRatio);
        } else {
            QMessageBox::information(this, tr("打开文件失败"), tr("打开文件失败!"));
        }
    }
}

void MainWindow::openDir()
{
    drawScene->unshowMagnify();
    drawScene->setCurrentShape(Shape::None);

    QFileDialog fileDiolog;
    fileDiolog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDiolog.setViewMode(QFileDialog::Detail);
    fileDiolog.setFileMode(QFileDialog::Directory);
    fileDiolog.setWindowTitle(tr("打开文件夹"));
    fileDiolog.setDirectory(QDir::homePath());

    if (fileDiolog.exec() != QDialog::Accepted) {
        QMessageBox::information(this, tr("打开文件夹失败"), tr("打开文件夹失败!"));
    } else {
        QString dir = fileDiolog.selectedFiles()[0];
        std::cout << dir.toStdString() << std::endl;
        runner = new Run(dir.toStdString());
        drawScene->setRunner(runner);
    }
}

void MainWindow::on_graphcutGammaSlider_valueChanged(int value)
{
    uiGamma = value;
    std::cout << value << std::endl;
}

void MainWindow::on_fgdRadius_valueChanged(int value)
{
    drawScene->setFgdRadius(value);
}

void MainWindow::on_bgdRadius_valueChanged(int value)
{
    drawScene->setBgdRadius(value);
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    drawScene->setOutlineRadius(value);
}

void MainWindow::on_foregroundTool_clicked()
{
    drawScene->setCurrentShape(Shape::Foreground);
}

void MainWindow::on_backgroundTool_clicked()
{
    drawScene->setCurrentShape(Shape::Background);
}

void MainWindow::on_defaultTool_clicked()
{
    drawScene->setCurrentShape(Shape::Fold);
}

void MainWindow::on_outlineTool_clicked()
{
    drawScene->clearOutline();
    drawScene->setCurrentShape(Shape::Outline);
}

void MainWindow::on_fwdOnceButton_clicked()
{
    if (runner != nullptr) {
        runner->forward();
        drawScene->setCurFrame(runner->getCurFrame());
        drawScene->showCurFrame();
        drawScene->clearUserPath();
    }
}

void MainWindow::on_fwdButton_clicked()
{
    if (runner != nullptr) {
        while (runner->forward()) {
            drawScene->setCurFrame(runner->getCurFrame());
            drawScene->showCurFrame();
            drawScene->clearUserPath();
        }
    }
}

void MainWindow::test_motion_estimation()
{
    OpencvTester b;
    cv::Mat img_1 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/keyframe-01.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat img_2 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/keyframe-02.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat mask = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/mask-1.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat img_out = cv::Mat::zeros(img_1.size(), CV_8UC3);

    QTime t;
    t.start();

    b.test_ORB_pos_estimation(img_1, img_2, mask, img_out);
    std::cout << "motion estimation: " << t.elapsed() << "ms" << std::endl;

    b.test_optical_flow(img_out, img_2);
    std::cout << "optical flow: " << t.elapsed() << "ms" << std::endl;
}

void MainWindow::test_graphcut()
{
    cv::Mat img_1 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/keyframe/keyframe-02.jpg", CV_LOAD_IMAGE_COLOR);
    // the reload of prob map may cause loss of precision
    cv::Mat img_2 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/p_2.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat mask_out, mask;
    cv::Mat im_out = cv::Mat(img_1.rows, img_1.cols, CV_8UC1);

    img_2.convertTo(img_2, CV_64FC1, 1.0 / 255.0);

    cv::imshow("input1", img_1);
    cv::imshow("input2", img_2);

    GraphCutTester a(img_1, img_2, mask, uiGamma);
    a.get_segmentation(im_out);

    img_1.copyTo(mask_out, im_out);

    cv::imshow("result_mask", im_out);
    cv::imshow("result", mask_out);
    cv::waitKey(0);
}

void MainWindow::test_alpha_matting()
{
    SharedMatting sm;
    sm.loadImage("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/keyframe-03.jpg");
    sm.loadTrimap("/Users/97littleleaf11/Desktop/hahahaha.png");
    sm.solveAlpha();
    sm.save("/Users/97littleleaf11/Desktop/gethahahahah.png");

    cv::Mat img_1 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/keyframe-03.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat alpha = cv::imread("/Users/97littleleaf11/Desktop/gethahahahah.png", CV_LOAD_IMAGE_COLOR);
    img_1.convertTo(img_1, CV_32FC3);
    alpha.convertTo(alpha, CV_32FC3, 1.0/255);
    multiply(alpha, img_1, img_1);
    cv::imwrite("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/alpht-cut-cut-03.png", img_1);
}

void MainWindow::get_cut()
{
    cv::Mat img_1 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/keyframe-01.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat img_2 = cv::imread("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/mask.png", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat mask_out;
    img_1.copyTo(mask_out, img_2);
    cv::imwrite("/Users/97littleleaf11/Documents/三 春夏 计算机图形学研究进展/book/keyframe/cut-01.png", mask_out);
}

void MainWindow::on_cutoutImageButton_clicked()
{
    drawScene->setCurrentShowMode(DrawScene::Seg);
    drawScene->showCurFrame();
}

void MainWindow::on_maskButton_clicked()
{
    drawScene->setCurrentShowMode(DrawScene::Cut);
    drawScene->showCurFrame();
}

void MainWindow::on_originalImageButton_clicked()
{
    drawScene->setCurrentShowMode(DrawScene::Origin);
    drawScene->showCurFrame();
}
