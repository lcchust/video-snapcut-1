#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsPixmapItem>

#include <drawscene.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open();
    void test_motion_estimation();
    void test_graphcut();

private:
    Ui::MainWindow *ui;

    DrawScene *drawScene;
    QImage *curImage;
    QGraphicsPixmapItem pixmapItem;

    QAction *openAction, *test1Action, *test2Action;
    QFileDialog *fileDiolog;
    QString path;
};

#endif // MAINWINDOW_H
