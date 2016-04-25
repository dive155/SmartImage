#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu * mnFile = new QMenu("File"); // создаём меню Файл
    // —-------— здесь добавляем пункт меню и подключаем его к слоту----
    QAction *opnAction = new QAction("Open",mnFile);
    QAction *saveAction = new QAction("Save",mnFile);
    //далее соединяем сигналы со слотами и добавляем действия в менюбар
    connect(opnAction, SIGNAL(triggered()), this, SLOT(loadImage()));
    mnFile->addAction(opnAction);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveImage()));
    mnFile->addAction(saveAction);
    ui->menuBar->addMenu(mnFile); // Добавляем пункты меню в menuBar, т.е. те, которые будут отображаться в гл. окне

    connect(ui->verticalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(vSliderChanged(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(hSliderChanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadImage() //загружаем картинку
{ //qDebug() << "loadimage";
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open"), "",
                                                    tr("Images") +
                                                       "(*.jpg *.jpeg *.png *.bmp "
                                                       "*.gif *.pbm *.pgm *.ppm "
                                                       "*.tiff *.xbm *.xpm);;" +
                                                    tr("All Files") +
                                                       "(*.*)");
    if (fileName.isEmpty())
        return;

    image = QImage(fileName);

    if (image.format() == QImage::Format_Invalid)
        return;
    result = QImage(image);
    showImage(&image);
    ui->horizontalSlider->setMaximumWidth(image.width()-1);
    ui->horizontalSlider->setMaximum(image.width()-1);
    ui->verticalSlider->setMaximumHeight(image.height()-1);
    ui->verticalSlider->setMaximum(image.height()-1);
    ui->horizontalSlider->setValue(image.width()/2);
    ui->verticalSlider->setValue(image.height()/2);
    //ui->cutLabel->setPixmap(doCut(&image,10));
}

void MainWindow::saveImage()
{//сохраняем картинку
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save"), "",
                                                                    tr("PNG") +
                                                                    "(*.png);;" +
                                                                    tr("All Files") +
                                                                    "(*.*)");
    if (fileName.isEmpty())
        return;
    result.save(fileName);
}

void MainWindow::showImage(QImage *picture)
{ //функция, отрисовывающая исходное изображение
    QPixmap pixmap; //картинку нельзя показать когда она в QImage
    pixmap.convertFromImage(*picture); //конвертируем картинку в pixmap
    ui->display->setPixmap(pixmap); //отображаем пиксмап в лейбле
}


QPixmap MainWindow::doCut(QImage *picture, int y)
{ //рисуем срез функции яркости
    QList<int> line; //заготовка под линию пикселей
    for (int x = 0; x < picture->width(); x++)
    {
        QColor pixel = picture->pixel(x, y);
        line << pixel.lightness(); //записываем пиксели
    }
    QImage tgist(picture->width(), 64, QImage::Format_RGB32);
    for (int x = 0; x < tgist.width(); x++)
    {//рисуем снизу вверх
        for (int y = 0; y < line[x]/4; y++)
        {   //сначала черным
            tgist.setPixel(x, y, qRgb(0,0,0));//pixel.rgb());
        }
        for (int y = line[x]/4; y < tgist.height(); y++)
        {   //потом белым
            tgist.setPixel(x, y, qRgb(255,255,255));//pixel.rgb());
        }
    }
    tgist = tgist.mirrored(0,1); //отзеркаливаем потому что пока что картинка перевернута
    QPixmap pixmap;
    pixmap.convertFromImage(tgist);
    return pixmap; //и возвращаем
}

QPixmap MainWindow::doCutVert(QImage *picture, int a)
{ //рисуем срез функции яркости
    QList<int> line; //заготовка под линию пикселей
    for (int y = 0; y < picture->height(); y++)
    {
        QColor pixel = picture->pixel(a, y);
        line << pixel.lightness(); //записываем пиксели
    }
    QImage tgist(64, picture->height(), QImage::Format_RGB32);
    //qDebug() << tgist.height() << tgist.width();
    for (int y = 0; y < tgist.height(); y++)
    {//рисуем снизу вверх
        if (y>=600) qDebug() << tgist.height() << tgist.width();
        for (int x = 0; x < line[y]/4; x++)
        {   //сначала черным
            tgist.setPixel(x, y, qRgb(0,0,0));//pixel.rgb());
        }
        for (int x = line[y]/4; x < tgist.width(); x++)
        {   //потом белым
            tgist.setPixel(x, y, qRgb(255,255,255));//pixel.rgb());
        }
    }
    tgist = tgist.mirrored(0,1); //отзеркаливаем потому что пока что картинка перевернута
    QPixmap pixmap;
    pixmap.convertFromImage(tgist);
    return pixmap; //и возвращаем
}

void MainWindow::vSliderChanged(int value)
{
    ui->horizCutLabel->setPixmap(doCut(&image, value));
}

void MainWindow::hSliderChanged(int value)
{
    ui->vertiCutLabel->setPixmap(doCutVert(&image, value));
}
