#ifndef BILATERATOR_H
#define BILATERATOR_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class Bilaterator;
}

class Bilaterator : public QWidget
{
    Q_OBJECT

public:
    explicit Bilaterator(QWidget *parent = 0);
    ~Bilaterator();
    void setSource(MainWindow* source);
    void setImage(QImage picture);
    qreal countGaussCoef(int centerValue, int pixValue, qreal coef);
    QList<qreal> genGauss(int power, qreal sigma);
    int applyKernel(QList<int> area, QList<qreal>  kernel);
    QList<int> vicinity(QImage *picture, int power, int curx, int cury, int chanel);
    QList<qreal> modifyGaussian(QList<int> area, QList<qreal>  kernel, qreal coef);

protected:
     MainWindow* window;

private:
    Ui::Bilaterator *ui;
    QImage image;
    QImage result;
    int strength, vertStrength;
    qreal sigma, vertSigma, sigmaCoef;

private slots:
    void setGausSigma(double spin);
    void checkSpin(int spin);
    void checkVertSigma(int spin);
    void applyFilter();

};

#endif // BILATERATOR_H
