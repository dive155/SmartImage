#ifndef COLORER_H
#define COLORER_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class Colorer;
}

class Colorer : public QWidget
{
    Q_OBJECT

public:
    explicit Colorer(QWidget *parent = 0);
    ~Colorer();
    void setSource(MainWindow* source);
    void setImage(QImage picture);
    QRgb countPixel(QColor pixel, int newHue, int newSat, int newLight);
    QRgb countPixelRGB(QColor pixel, int newRed, int newGreen, int newBlue);
    void dropSliders();
    int linear(int source, int slideOne, int slideTwo);

protected:
     MainWindow* window;

private:
    Ui::Colorer *ui;
    QImage image;
    QImage result;
    int hueSet, satuSet, lightSet;
    int redSet, greenSet, blueSet;
    int autoThreshold;


private slots:
    void hueChanged(int value);
    void satuChanged(int value);
    void lightnessChanged(int value);
    void invertSlot();
    void bnwSlot();

    void redChanged(int value);
    void greenChanged(int value);
    void blueChanged(int value);

    void autoBalanceSlot();
    void thresholdSlot(int value);

    void greyMethod();
};

#endif // COLORER_H
