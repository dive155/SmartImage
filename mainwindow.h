#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QFileDialog>
#include <QtDebug>
#include <QtGui/QMouseEvent>

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


private:
    Ui::MainWindow *ui;
    QImage image;
    QImage result;
    QImage output;


private slots:
void loadImage();
void saveImage();
void showImage(QImage *picture);
void vSliderChanged(int value);
void hSliderChanged(int value);
};

#endif // MAINWINDOW_H
