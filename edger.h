#ifndef EDGER_H
#define EDGER_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class Edger;
}

class Edger : public QWidget
{
    Q_OBJECT

public:
    explicit Edger(QWidget *parent = 0);
    ~Edger();
    void setSource(MainWindow* source);
    void setImage(QImage picture);
    void doBNW(QImage &target);
    void filtPixel(int curx, int cury, int mode);
    int applyKirschKernel(QList<int> area, QList<QList<int> > kernel);
    int applyKernel(QList<int> area, QList<int>  kernelx, QList<int>  kernely);
    int applyWallace(QList<int> area);
    double maxOfList(QList<int> aList);

protected:
     MainWindow* window;

private:
    Ui::Edger *ui;
    QImage image;
    QImage result;
    QImage blackNWhite;
    int filter;
    QList<int> preCoef;
    double coef;

    QList<QList<int> > kernels;
    QList<int> sobelx; QList<int> sobely;
    QList<int> prewitx; QList<int> prewity;

private slots:
    void doFilter();
    void radio();
};

#endif // EDGER_H
