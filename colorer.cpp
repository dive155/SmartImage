#include "colorer.h"
#include "ui_colorer.h"

Colorer::Colorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Colorer)
{
    ui->setupUi(this);

    connect(ui->hueSlider, SIGNAL(valueChanged(int)),
            this, SLOT(hueChanged(int)));
    connect(ui->satuSlider, SIGNAL(valueChanged(int)),
            this, SLOT(satuChanged(int)));
    connect(ui->lightnessSlider, SIGNAL(valueChanged(int)),
            this, SLOT(lightnessChanged(int)));
    connect(ui->invertButton, SIGNAL(clicked()),
            this, SLOT(invertSlot()));
    connect(ui->bnwButton, SIGNAL(clicked()),
            this, SLOT(bnwSlot()));
    connect(ui->balanceButton, SIGNAL(clicked()),
            this, SLOT(autoBalanceSlot()));
    connect(ui->greyButton, SIGNAL(clicked()),
            this, SLOT(greyMethod()));

    connect(ui->redSlider, SIGNAL(valueChanged(int)),
            this, SLOT(redChanged(int)));
    connect(ui->greenSlider, SIGNAL(valueChanged(int)),
            this, SLOT(greenChanged(int)));
    connect(ui->blueSlider, SIGNAL(valueChanged(int)),
            this, SLOT(blueChanged(int)));
    connect(ui->thresholdSlider, SIGNAL(valueChanged(int)),
            this, SLOT(thresholdSlot(int)));
    hueSet=0;
    satuSet=0;
    lightSet=0;
    redSet = 0;
    greenSet = 0;
    blueSet = 0;
    autoThreshold=5;
}

Colorer::~Colorer()
{
    delete ui;
}

void Colorer::setSource(MainWindow *source)
{
    window = source;
}

void Colorer::setImage(QImage picture)
{
    image = picture;
    result = image;
}

void Colorer::hueChanged(int value)
{ //слот, реагирующий на движение слайдера цветового тона
    ui->hueLabel->setText(QString("%1").arg(value));
    hueSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixel(image.pixel(x,y), hueSet, satuSet, lightSet));
        }
    window->receiveResult(result);
}

void Colorer::satuChanged(int value)
{ //слот, реагирующий на движение слайдера насыщенности
    ui->satuLabel->setText(QString("%1").arg(value));
    satuSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixel(image.pixel(x,y), hueSet, satuSet, lightSet));
        }
    window->receiveResult(result);
}

void Colorer::lightnessChanged(int value)
{ //слот, реагирующий на движение слайдера яркости
    ui->lightnessLabel->setText(QString("%1").arg(value));
    lightSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixel(image.pixel(x,y), hueSet, satuSet, lightSet));
        }
    window->receiveResult(result);
}

void Colorer::bnwSlot()
{ //слот который делает картинку чёрнобелой
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            QColor pixel = image.pixel(x, y); //получаем пиксель и затем записываем значение с измененной яркостью
            pixel.setHsl(pixel.hslHue(),0,pixel.lightness());
            result.setPixel(x,y,pixel.rgb());
        }
    window->receiveResult(result);
}

void Colorer::redChanged(int value)
{//поменян слайдер красного
    ui->redLabel->setText(QString("%1").arg(value));
    redSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixelRGB(image.pixel(x,y), redSet, greenSet, blueSet));
        }
    window->receiveResult(result);
}

void Colorer::greenChanged(int value)
{//поменян слайдер зеленого
    ui->greenLabel->setText(QString("%1").arg(value));
    greenSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixelRGB(image.pixel(x,y), redSet, greenSet, blueSet));
        }
    window->receiveResult(result);
}

void Colorer::blueChanged(int value)
{ //поменян слайдер синего
    ui->blueLabel->setText(QString("%1").arg(value));
    blueSet = value;
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            result.setPixel(x,y, countPixelRGB(image.pixel(x,y), redSet, greenSet, blueSet));
        }
    window->receiveResult(result);
}


void Colorer::invertSlot()
{ //слот который делает картинку инвертированной
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            QColor pixel = image.pixel(x,y);
            pixel.setHsl(pixel.hslHue()+180, pixel.hslSaturation(), 255-pixel.lightness());
            result.setPixel(x,y,pixel.rgb());
        }
    window->receiveResult(result);
}

QRgb Colorer::countPixel(QColor pixel, int newHue, int newSat, int newLight)
{//слот цветокоррекции через цветовой тон/насыщенность/яркость для одного пикселя
    newHue = pixel.hue()+newHue;
    if (newHue<0)
        newHue = 359 + newHue;
    newSat = pixel.saturation()+newSat;
    if (newSat>255)
        newSat=255;
    if (newSat<0)
        newSat=0;
    newLight = pixel.lightness()+newLight;
    if (newLight>255)
        newLight=255;
    if (newLight<0)
        newLight=0;


    pixel.setHsl(newHue, newSat, newLight);
    return pixel.rgb();
}

void Colorer::dropSliders()
{ //обнуление слайдеров
    ui->hueSlider->setValue(0);
    ui->satuSlider->setValue(0);
    ui->lightnessSlider->setValue(0);
    ui->redSlider->setValue(0);
    ui->greenSlider->setValue(0);
    ui->blueSlider->setValue(0);
}

QRgb Colorer::countPixelRGB(QColor pixel, int newRed, int newGreen, int newBlue)
{//слот цветокоррекции через цветовой тон/насыщенность/яркость для одного пикселя
    newRed = pixel.red()+newRed;
    if (newRed>255)
        newRed=255;
    if (newRed<0)
        newRed=0;
    newGreen = pixel.green()+newGreen;
    if (newGreen>255)
        newGreen=255;
    if (newGreen<0)
        newGreen=0;
    newBlue = pixel.blue()+newBlue;
    if (newBlue>255)
        newBlue=255;
    if (newBlue<0)
        newBlue=0;


    pixel.setRgb(newRed, newGreen, newBlue);
    return pixel.rgb();
}


void Colorer::thresholdSlot(int value)
{ //слот порога
    autoThreshold = value;
    ui->thresholdLabel->setText(QString("%1").arg(value));
}

void Colorer::greyMethod()
{   //автобаланс методом серого мира
    qreal sum = 0; //считается что всё должно быть серое
    qreal redSum = 0;
    qreal greenSum = 0;
    qreal blueSum = 0;
    QColor pixel;
    for (int y = 0; y<image.height(); y++)
       for (int x = 0; x<image.width(); x++)
       {
           sum++;
           pixel = image.pixel(x,y);
           redSum = redSum+pixel.red();
           greenSum = greenSum+pixel.green();
           blueSum = blueSum + pixel.blue();
       }
    redSum = redSum/sum;
    greenSum = greenSum/sum;
    blueSum = blueSum/sum; //считаем средние по цветам
    sum = (redSum+greenSum+blueSum)/3.0; //и вообще среднее
    for (int y = 0; y<image.height(); y++)
       for (int x = 0; x<image.width(); x++)
       { //считаем новый цвет
           qreal red = redSum;
           qreal green = greenSum;
           qreal blue = blueSum;

           pixel = image.pixel(x,y);
           red=pixel.red()*(sum/redSum);
           green= pixel.green()*(sum/greenSum);
           blue=pixel.blue()*(sum/blueSum);

           if (red>255) red=255;
           if (green>255) green=255;
           if (blue>255) blue=255;
           //qDebug() << blue;
           if (red<0) red=0;
           if (green<0) green=0;
           if (blue<0) blue=0;

           pixel.setRed(red);
           pixel.setGreen(green);
           pixel.setBlue(blue);
           result.setPixel(x,y,pixel.rgb());
       }
    window->receiveResult(result);
}

void Colorer::autoBalanceSlot()
{//автобаланс методом линейного растяжения гистограмм цветовых каналов
    int threshold = autoThreshold; //порог восприятия
    window->makeGist(image, 1);
    QList<int>redArray = window->retrieveAray();
    int minRed = 0;
    int maxRed = 255;
    for (int i = 0; i<redArray.size(); i++)
    { //ищем минимумы и максимумы для растяжения гистограмм
        if (redArray[i]>threshold)
        {
            minRed = i;
            break;
        }
    }
    for (int i = redArray.size()-1; i>=0; i--)
    {
        if (redArray[i]>threshold)
        {
            maxRed = i;
            break;
        }
    }
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        { //применяем растяжение
            QColor pixel = image.pixel(x,y);
            pixel.setRed(linear(pixel.red(),minRed, maxRed));
            result.setPixel(x,y,pixel.rgb());
        }
    /*-------------------------------------------------------------------*/
    window->makeGist(image, 2);
    QList<int>greenArray = window->retrieveAray();
    int minGreen = 0;
    int maxGreen = 255;
    for (int i = 0; i<greenArray.size(); i++)
    {
        if (greenArray[i]>threshold)
        {
            minGreen = i;
            break;
        }
    }
    for (int i = greenArray.size()-1; i>=0; i--)
    {
        if (greenArray[i]>threshold)
        {
            maxGreen = i;
            break;
        }
    }
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            QColor pixel = image.pixel(x,y);
            pixel.setGreen(linear(pixel.green(),minGreen, maxGreen));
            result.setPixel(x,y,pixel.rgb());
        }
    /*-------------------------------------------------------------------*/
    window->makeGist(image, 3);
    QList<int>blueArray = window->retrieveAray();
    int minBlue = 0;
    int maxBlue = 255;
    for (int i = 0; i<blueArray.size(); i++)
    {
        if (blueArray[i]>threshold)
        {
            minBlue = i;
            break;
        }
    }
    for (int i = blueArray.size()-1; i>=0; i--)
    {
        if (blueArray[i]>threshold)
        {
            maxBlue = i;
            break;
        }
    }
    for (int y = 0; y<image.height(); y++)
        for (int x = 0; x<image.width(); x++)
        {
            QColor pixel = image.pixel(x,y);
            pixel.setBlue(linear(pixel.blue(),minBlue, maxBlue));
            result.setPixel(x,y,pixel.rgb());
        }
    window->receiveResult(result);
}

int Colorer::linear(int source, int slideOne, int slideTwo)
{ //функция принимает значение яркости пикселя и возвращает значение,
    //равное результату линйного преобразования с текущими настройками
    if (source >= slideOne && source<slideTwo) //если в диапазоне то считаем по формуле
        return (255.0/(slideTwo-slideOne)) * (source-slideOne);
    if (source < slideOne) //если меньше диапазона то возвращаем 0
        return 0;
    else //если больше то 255
        return 255;
}
