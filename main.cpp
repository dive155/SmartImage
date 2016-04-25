#include "mainwindow.h"
#include "simpleaver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowTitle("Smart Image");
    return a.exec();
}
