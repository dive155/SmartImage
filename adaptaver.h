#ifndef ADAPTAVER_H
#define ADAPTAVER_H

#include <QWidget>
#include <mainwindow.h>
class MainWindow;

namespace Ui {
class AdaptAver;
}

class AdaptAver : public QWidget
{
    Q_OBJECT

public:
    explicit AdaptAver(QWidget *parent = 0);
    ~AdaptAver();
    void setSource(MainWindow* source);
    void setImage(QImage picture);

    void adArAv(int curx, int cury, int powerX, int powerY);
    int listAverage(QList<int> theArray);
    double listDisp(QList<int> theArray, int mu);

    QList<int> vicinity(QImage *picture, int power, int curx, int cury, int chanel);
    int median(QList<int> src);
    int listMax(QList<int> src);
    int listMin(QList<int> src);
    void adaptMedian(int curx, int cury, int maxR);
protected:
     MainWindow* window;

private:
    Ui::AdaptAver *ui;
    QImage image;
    QImage result;
    int strength;
    int dispNoise;
    int limit;

private slots:
    void checkSpin(int spin);
    void checkVariance(int spin);
    void adArAvFilter();
    void adaptFilter();
    void checkLimit(int spin);
};

#endif // ADAPTAVER_H
