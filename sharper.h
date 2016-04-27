#ifndef SHARPER_H
#define SHARPER_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class Sharper;
}

class Sharper : public QWidget
{
    Q_OBJECT

protected:
     MainWindow* window;

public:
    explicit Sharper(QWidget *parent = 0);
    ~Sharper();
    void setSource(MainWindow* source);
    void setImage(QImage picture);

    QList<int> vicinity(QImage *picture, int power, int curx, int cury, int chanel);
    int listAverage(QList<int> theArray);
    double listDisp(QList<int> theArray, int mu);
    int applyKernel(QList<int> area, QList<qreal>  kernel);
    void uncertainMask(QImage *picture, int mode, int hsl);
    double countKg(QList<int> proxy);
    void localContrast(QImage *picture);
    int contrastPixel(int source, int proxBright);
    QPixmap doCut(QImage *picture, int y);
    QList<qreal> genGauss(int power, qreal sigma);
    QColor uncertainColor(QImage *picture, int mode, int tempKg, QColor sourcePixel, QList<qreal>gaussian, int x, int y, int hsl);


private:
    Ui::Sharper *ui;
    QImage image;
    QImage result;
    QImage output;
    int kg, kn, contArea, realKg;
    int power;
    int gausPower;
    qreal gausSigma;
    double N;

private slots:
    void kgChanged(int spin);
    void uncertainSlot();
    void sliderChanged(int value);
    void nChanged(double spin);
    void contrastSlot();
    void contAreaChanged(int spin);

    void realKgChanged(int spin);
    void simpleSharp();
    void simpleSharpRGB();
    void setGausPower(int spin);
    void setGausSigma(double spin);
};

#endif // SHARPER_H
