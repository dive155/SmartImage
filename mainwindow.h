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
#include "edger.h"
#include "colorer.h"
#include "bilaterator.h"
#include "helpwindow.h"
#include <QDesktopWidget>

class SimpleAver;
class AdaptAver;
class Curver;
class Sharper;
class Edger;
class Colorer;
class Bilaterator;

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
    QPixmap makeGist(QImage &greyPic, int mode);
    void drawCurveOnGist(QList<int> sourceArray);
    QList<int> retrieveAray();
    void setupEverything(QImage image, QImage imageScaled);
    QDesktopWidget desktop;
    QImage reduceSize(QImage picture);

private:
    Ui::MainWindow *ui;
    QImage image;
    QImage result;
    QImage output;
    QImage imageScaled;
    QImage resultScaled;
    int array[256];
    QPixmap sourceGist, resultGist;
    QList<int> funcArray;
    int maxWidth;
    int maxHeight;

    SimpleAver* sAver;
    AdaptAver* aAver;
    Curver* curver;
    Sharper* sharper;
    Edger* edger;
    Colorer* colorer;
    Bilaterator* bilaterator;

    int cutx,cuty;

    HelpWindow* whatIsThis;

private slots:
void loadImage();
void saveImage();
void showImage(QImage *picture);
void vSliderChanged(int value);
void hSliderChanged(int value);
void radio();
void saveSlot();
void makeAbout();
};

#endif // MAINWINDOW_H
