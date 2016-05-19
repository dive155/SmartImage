#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu * mnFile = new QMenu("Файл"); // создаём меню Файл
    // —-------— здесь добавляем пункт меню и подключаем его к слоту----
    QAction *opnAction = new QAction("Открыть",mnFile);
    QAction *saveAction = new QAction("Сохранить",mnFile);
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
    connect(ui->sourceBtn, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    connect(ui->resultBtn, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(saveSlot()));

    ui->toolBox->removeItem(0);
    ui->toolBox->removeItem(0);

    qDebug() << "been here";
    QPixmap pixmap = QPixmap(":placeholders/background.png");
    image = pixmap.toImage();
    result = image;
    setupEverything(image);
    sourceGist=makeGist(image,4);
    ui->gistLabel->setPixmap(sourceGist);


    colorer = new Colorer();
    ui->toolBox->addItem(colorer, "Цветокоррекция");
    colorer->setSource(this);
    colorer->setImage(image);

    curver = new Curver();
    ui->toolBox->addItem(curver, "Кривые");
    curver->setSource(this);
    curver->setImage(image);

    sAver = new SimpleAver();
    ui->toolBox->addItem(sAver, "Простое шумоподавление");
    sAver->setSource(this);
    sAver->setImage(image);

    aAver = new AdaptAver();
    ui->toolBox->addItem(aAver, "Адаптивное шумоподавление");
    aAver->setSource(this);
    aAver->setImage(image);

    bilaterator = new Bilaterator();
    ui->toolBox->addItem(bilaterator, "Билатеральный фильтр");
    bilaterator->setSource(this);
    bilaterator->setImage(image);

    sharper = new Sharper();
    ui->toolBox->addItem(sharper, "Увеличение резкости");
    sharper->setSource(this);
    sharper->setImage(image);


    edger = new Edger();
    ui->toolBox->addItem(edger, "Выделение граней");
    edger->setSource(this);
    edger->setImage(image);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete edger;
    delete sharper;
    delete bilaterator;
    delete aAver;
    delete sAver;
    delete curver;
    delete colorer;
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
    setupEverything(image);
    sourceGist=makeGist(image,4);
    ui->gistLabel->setPixmap(sourceGist);

    sAver->setImage(image);
    aAver->setImage(image);
    curver->setImage(image);
    sharper->setImage(image);
    edger->setImage(image);
    colorer->setImage(image);
    bilaterator->setImage(image);
}

void MainWindow::setupEverything(QImage image)
{ //настраиваем классы и интерфейс при загрузке новой картинки
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
    cuty=y;
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
    cutx=a;
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
        //if (y>=600) qDebug() << tgist.height() << tgist.width();
        for (int x = 0; x < line[y]/4; x++)
        {   //сначала черным
            tgist.setPixel(x, y, qRgb(0,0,0));//pixel.rgb());
        }
        for (int x = line[y]/4; x < tgist.width(); x++)
        {   //потом белым
            tgist.setPixel(x, y, qRgb(255,255,255));//pixel.rgb());
        }
    }
    QPixmap pixmap;
    pixmap.convertFromImage(tgist);
    return pixmap; //и возвращаем
}

QPixmap MainWindow::makeGist(QImage &greyPic, int mode)
{//функция, пересчитывающая яркости пикселей и выдающая гистограмму
    QImage gist(256,128, QImage::Format_RGB32);

    for (int z=1; z<3; z++)
    {
        for (int j = 0; j < 255; j++)
        { //сначала всех пикселей ноль штук
            array[j] = 0;
        };
        for (int x = 0; x < greyPic.width(); x++)
            for (int y = 0; y < greyPic.height(); y++)
            { //для каждого пикселя
                QColor pixel = greyPic.pixel(x, y); //получаем пиксель
                int color;
                if (mode == 1)
                    color = pixel.red(); //считываем его яркость в переменную color
                if (mode == 2)
                    color = pixel.green(); //считываем его яркость в переменную color
                if (mode == 3)
                    color = pixel.blue(); //считываем его яркость в переменную color
                else
                    color = pixel.lightness(); //считываем его яркость в переменную color
                array[color] = array[color]+1; //и увеличиваем счетчик соответствующей яркости на 1
            }
        int big = 0; //найдем самый большой элемент массива
        for (int m = 0; m < 256; m++)
            {
                if (array[m] >= big)
                        big = array[m]; //и запишем его в переменную big
            } //теперь нам нужно нормализовать массив чтобы самый большой элемент не превысил 255
        double coef = 0;
        coef = 128.0 / big; //для этого посчитали нормализующий коэффициент
        for (int m = 0; m < 256; m++)
             {
                array[m] = array[m]*coef; //применяем его ко всем элементам
             }
        for (int x = 0; x < gist.width(); x++)
            for (int y = 0; y < gist.height(); y++)
            { // сначала закрашиваем её белым цветом
                QColor pixel = QColor(255,255,255);//gist.pixel(x, y);
                gist.setPixel(x, y, pixel.rgb());
            }
        for (int x = 0; x < gist.width(); x++)
            if (array[x]>>0) //теперь нанесем черные столбцы толщиной в 1 пиксель
            for (int y = 0; y < array[x]; y++) //столбцы имеют высоту от нуля до значения распространенности соответствующей яркости
            { //т.е. например если 50% всех пикселей имеют яркость 120 то 120й столбик будет иметь высоту в 50% * 255 = 127
                QColor pixel = QColor(0,0,0);
                gist.setPixel(x, y, pixel.rgb());
            }

        gist = gist.mirrored(0,1); //зеркалим гистограмму чтобы не была вверх ногами
    }
    QPixmap pixmap; //картинку нельзя показать когда она в QImage
    pixmap.convertFromImage(gist); //конвертируем картинку в pixmap
    return pixmap;
}

void MainWindow::drawCurveOnGist(QList<int> sourceArray)
{
    QImage gist = sourceGist.toImage();
    for (int x = 0; x < gist.width(); x++)
    { //сверху синей линией нанесём чистый график функции
        QColor pixel = gist.pixel(x, 10);
        pixel.setRed(0);
        pixel.setGreen(128);
        pixel.setBlue(255);
        gist.setPixel(x,sourceArray[x]/2, pixel.rgb());
    }
    QPixmap pixmap; //картинку нельзя показать когда она в QImage
    pixmap.convertFromImage(gist); //конвертируем картинку в pixmap
    ui->gistLabel->setPixmap(pixmap);
}

QList<int> MainWindow::retrieveAray()
{ //функция возвращает массив array в виде QList
    QList<int> list;
    for (int i=1; i<256; i++)
    {
        list << array[i];
    }
    return list;
}


void MainWindow::vSliderChanged(int value)
{
    QImage* link;
    if (ui->sourceBtn->isChecked()==true)
        link = &image;
    else
        link = &result;
    ui->horizCutLabel->setPixmap(doCut(link, value));
}

void MainWindow::hSliderChanged(int value)
{
    QImage* link;
    if (ui->sourceBtn->isChecked()==true)
        link = &image;
    else
        link = &result;
    ui->vertiCutLabel->setPixmap(doCutVert(link, value));
}

void MainWindow::radio()
{ //функция срабатывающая при переключении на исходное изображение
    if (ui->sourceBtn->isChecked())
    {
        output = image;
        showImage(&image); //показываем картинку
        ui->horizCutLabel->setPixmap(doCut(&image,cuty));
        ui->vertiCutLabel->setPixmap(doCutVert(&image,cutx));
        ui->gistLabel->setPixmap(sourceGist);
    }
    if (ui->resultBtn->isChecked())
    {
        output = result;
        showImage(&result); //показываем картинку
        ui->horizCutLabel->setPixmap(doCut(&result,cuty));
        ui->vertiCutLabel->setPixmap(doCutVert(&result,cutx));
        ui->gistLabel->setPixmap(resultGist);
    }

}

void MainWindow::saveSlot()
{ //сохранить изменения, внесенные фильтрами
    image=result;
    showImage(&image);
    ui->horizCutLabel->setPixmap(doCut(&result,cuty));
    ui->vertiCutLabel->setPixmap(doCutVert(&result,cutx));
    resultGist=makeGist(result,4);
    sourceGist=resultGist;
    ui->gistLabel->setPixmap(resultGist);

    sAver->setImage(image);
    aAver->setImage(image);
    curver->setImage(image);
    sharper->setImage(image);
    edger->setImage(image);
    colorer->setImage(image);
    bilaterator->setImage(image);

    colorer->dropSliders();
}


void MainWindow::receiveResult(QImage picture)
{ //получить результат
    result = picture;
    showImage(&result);
    ui->resultBtn->setChecked(true);
    ui->horizCutLabel->setPixmap(doCut(&result,cuty));
    ui->vertiCutLabel->setPixmap(doCutVert(&result,cutx));

    resultGist=makeGist(result,4);
    sourceGist=makeGist(image,4);
    ui->gistLabel->setPixmap(resultGist);
}

void MainWindow::setMaxProgress(int value)
{
    ui->progressBar->setMaximum(value);
}

void MainWindow::setProgress(int value)
{
    ui->progressBar->setValue(value);
    qApp->processEvents();
}
