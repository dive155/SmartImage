#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QFileDialog>
#include <QtDebug>
#include <QtGui/QMouseEvent>
#include <QtMath>
#include "simpleaver.h"
#include "adaptaver.h"
#include "curver.h"
#include "sharper.h"

class SimpleAver;
class AdaptAver;
class Curver;
class Sharper;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QPixmap doCut(QImage *picture, int y);
    QPixmap doCutVert(QImage *picture, int a);
    void receiveResult(QImage picture);
    void setMaxProgress(int value);
    void setProgress(int value);
    QPixmap makeGist(QImage &greyPic);
    void drawCurveOnGist(QList<int> sourceArray);


private:
    Ui::MainWindow *ui;
    QImage image;
    QImage result;
    QImage output;
    int array[256];
    QPixmap sourceGist, resultGist;
    QList<int> funcArray;

    SimpleAver* sAver;
    AdaptAver* aAver;
    Curver* curver;
    Sharper* sharper;

    int cutx,cuty;

private slots:
void loadImage();
void saveImage();
void showImage(QImage *picture);
void vSliderChanged(int value);
void hSliderChanged(int value);
void radio();
void saveSlot();
};

#endif // MAINWINDOW_H
