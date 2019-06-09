#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);

    cv::Mat inputImage = cv::imread("/Users/97littleleaf11/Desktop/test.png");
    if (!inputImage.empty())
        cv::imshow("Display Image", inputImage);
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
        path = fileDiolog.selectedFiles()[0];
        if (path.length()) {
            std::cout << path.toStdString() << std::endl;
        } else {
            QMessageBox::information(this, tr("打开文件失败"), tr("打开文件失败!"));
        }
    }
}
