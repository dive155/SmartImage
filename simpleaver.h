#ifndef SIMPLEAVER_H
#define SIMPLEAVER_H

#include <QWidget>

#include <mainwindow.h>
class MainWindow;

namespace Ui {
class SimpleAver;
}

class SimpleAver : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleAver(QWidget *parent = 0);
    ~SimpleAver();
     void xRang(int curx, int cury, int power);
     void Rangovy(int curx, int cury, int power, int k);
     void setSource(MainWindow* source);
     void setImage(QImage picture);
     void arAv(int curx, int cury, int powerX, int powerY);
     int listAverage(QList<int> theArray);

     void doSredne(int curx, int cury, int power);
     QList<int> vicinity(QImage *picture, int power, int curx, int cury, int chanel);
     qreal sredne(QList<int> src);
     void doContr(int curx, int cury, int power, int rang);
     qreal contr(QList<int> src, int rang);

protected:
     MainWindow* window;


private:
    Ui::SimpleAver *ui;
    QImage image;
    QImage result;
    QImage gist;
    int strength;
    int displacement;
    int degree;

private slots:
    void medFilter();
    void rangFilter();
    void checkSpin(int spin);
    void checkK();
    void checkDisplacement(int spin);
    void arAvFilter();

    void sredneFilter();
    void contrFilter();
    void checkSpinD(int spin);
    void checkdegree(int spin);
};

#endif // SIMPLEAVER_H
