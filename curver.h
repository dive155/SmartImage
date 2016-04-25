#ifndef CURVER_H
#define CURVER_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class Curver;
}

class Curver : public QWidget
{
    Q_OBJECT

public:
    explicit Curver(QWidget *parent = 0);
    ~Curver();
    void setSource(MainWindow* source);
    void setImage(QImage picture);

    int linear(int source);
    void doLinear();
    QList<int> countCurve(int mode);

    int gamma(int source, double value);
    int solarise(int source, double gamma);
    void doGamma(int x, int y);
    void doSolar(int x, int y);
    void newCycle(int mode);
    int shiftedZ(int source, int aver, double powa);
    int averageZ();
    void doShifted(int x, int y, int aver);

protected:
     MainWindow* window;

private:
    Ui::Curver *ui;
    int slideOne;
    int slideTwo;
    QImage image;
    QImage result;

    int solValue;
    double gammaValue;
    int shiftValue;
    double shiftPower;
    bool correct;

private slots:
    void partLinearSlot();
    void sliderChanged(int value);
    void sliderChanged2(int value);

    void gammaSlot();
    void solarSlot();
    void shiftedSlot();
    void checkSol(int value);
    void checkGamma(double value);
    void sliderChanged3(int value);
    void checkboxChanged(int);
    void checkOne();
    void checkShift(int value);
};

#endif // CURVER_H
