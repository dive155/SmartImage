#include "curver.h"
#include "ui_curver.h"

Curver::Curver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Curver)
{
    ui->setupUi(this);

    slideOne = 0; //минимальное и максимальное значение диапазона
    slideTwo = 255;
    //далее настраиваем интерфейс
    ui->slider1->setMinimum(0);
    ui->slider1->setMaximumWidth(256);
    ui->slider1->setMaximum(255);
    ui->slider1->setValue(0);
    ui->slider2->setMinimum(0);
    ui->slider2->setMaximum(255);
    ui->slider2->setMaximumWidth(256);
    ui->slider2->setValue(255);

    //связываем сигналы со слотами (кнопки с функциями)
    connect(ui->partLinear, SIGNAL(clicked()),
            this, SLOT(partLinearSlot()));
    connect(ui->slider1, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged(int)));
    connect(ui->slider2, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged2(int)));

    solValue = 2; //степень соляризации
    gammaValue = 0.5; //степень гамма-коррекции

    ui->spinBox->setMinimum(2);
    ui->spinBox->setSingleStep(2);
    ui->spinBox->setMaximum(12);

    ui->doubleSpinBox->setValue(0.5);
    ui->doubleSpinBox->setSingleStep(0.1);
    ui->doubleSpinBox->setMinimum(0.1);
    ui->doubleSpinBox->setMaximum(10);

    shiftPower = 3; //степень степенного преобразования
    //далее настраиваем интерфейс
    ui->nlo->setChecked(true);
    ui->shiftedSpinbox->setMinimum(1);
    ui->shiftedSpinbox->setMaximum(15);
    ui->shiftedSpinbox->setSingleStep(2);
    ui->shiftedSpinbox->setValue(3);


    connect(ui->gammaButton, SIGNAL(clicked()),
            this, SLOT(gammaSlot()));
    connect(ui->solarButton, SIGNAL(clicked()),
            this, SLOT(solarSlot()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkSol(int)));
    connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(checkGamma(double)));

    connect(ui->shiftedButton, SIGNAL(clicked()),
            this, SLOT(shiftedSlot()));
    connect(ui->shiftedSpinbox, SIGNAL(valueChanged(int)),
            this, SLOT(checkShift(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged3(int)));
    connect(ui->checkBox, SIGNAL(stateChanged(int)),
            this, SLOT(checkboxChanged(int)));

    connect(ui->nlo, SIGNAL(toggled(bool)),
            this, SLOT(checkOne()));
    connect(ui->ngo, SIGNAL(toggled(bool)),
            this, SLOT(checkOne()));

}

Curver::~Curver()
{
    delete ui;
}

void Curver::setSource(MainWindow *source)
{
    window = source;
}

void Curver::setImage(QImage picture)
{
    image = picture;
    result = image;
}

QList<int> Curver::countCurve(int mode)
{
    QList<int> funcArray;
    funcArray.clear(); //очищаем массив
    for (int i = 0; i<256; i++)
    { //заполняем в соответствие с выбраным фильтром
        if (mode == 1) funcArray.append(gamma(i,gammaValue));
        if (mode == 2) funcArray.append(solarise(i, solValue));
        if (mode == 3) funcArray.append(shiftedZ(i,shiftValue,shiftPower));
        if (mode == 4) funcArray.append(linear(i));
        funcArray[i] = 255 - funcArray[i];
    }
    return funcArray;
}

int Curver::linear(int source)
{ //функция принимает значение яркости пикселя и возвращает значение,
    //равное результату линйного преобразования с текущими настройками
    if (source >= slideOne && source<slideTwo) //если в диапазоне то считаем по формуле
        return (255.0/(slideTwo-slideOne)) * (source-slideOne);
    if (source < slideOne) //если меньше диапазона то возвращаем 0
        return 0;
    else //если больше то 255
        return 255;
}

void Curver::doLinear()
{//функция, применяющая фильтр к каждому пикселю изображения
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    { //для каждой строки
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        {//для каждого пикселя
            QColor pixel = image.pixel(x, y); //получаем пиксель и затем записываем значение с измененной яркостью
            pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),linear(pixel.lightness()));
            result.setPixel(x,y,pixel.rgb()); //сохраняем результат
        }
    }
    window->receiveResult(result); //показываем результат
}


void Curver::partLinearSlot()
{ //слот вызывающий кусочно-линейное преобразование
    doLinear(); //обрабатываем картинку
}


void Curver::sliderChanged(int value)
{ //слот, реагирующий на движение слайдера
    slideOne = qMin(value, ui->slider2->value()); //считаем новые границы диапазона
    slideTwo = qMax(value, ui->slider2->value());
    ui->rangeLable->setText(QString("%1-%2").arg(slideOne).arg(slideTwo)); //выводим числа в лейбл
    window->drawCurveOnGist(countCurve(4));
}

void Curver::sliderChanged2(int value)
{//слот, реагирующий на движение второго слайдера
    slideOne = qMin(value, ui->slider1->value()); //считаем новые границы диапазона
    slideTwo = qMax(value, ui->slider1->value());
    ui->rangeLable->setText(QString("%1-%2").arg(slideOne).arg(slideTwo));//выводим числа в лейбл
    window->drawCurveOnGist(countCurve(4));
}


/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/



void Curver::gammaSlot()
{ //слот вызывающий функцию гамма коррекции
    newCycle(1); //обрабатываем картинку

}

void Curver::solarSlot()
{ //слот вызывающий функцию соляризации
    newCycle(2);//обрабатываем картинку

}

void Curver::shiftedSlot()
{ //слот вызывающий функцию степенного преобразования
    newCycle(3);//обрабатываем картинку
}

void Curver::checkShift(int value)
{ //слот меняющий степень степенного преобразования
    shiftPower = value;
    window->drawCurveOnGist(countCurve(3));
}

void Curver::checkOne()
{ //слот - переключатель режимов работы степенного преобразования (n<1 и n>1)
    if (ui->nlo->isChecked())
    {
        ui->label_6->setText("n = 1/N, N=");
        correct = false;
        window->drawCurveOnGist(countCurve(3));
    }
    if (ui->ngo->isChecked())
    {
        ui->label_6->setText("n = N, N=");
        correct = true;
        window->drawCurveOnGist(countCurve(3));
    }
}

void Curver::checkSol(int value)
{ //слот меняющий степень соляризации
    solValue = value;
    window->drawCurveOnGist(countCurve(2));
}

void Curver::checkGamma(double value)
{ //слот меняющий степень гамма-коррекции
    gammaValue = value;
    window->drawCurveOnGist(countCurve(1));
}


void Curver::sliderChanged3(int value)
{//слот изменяющий значение смещенного нуля для степенного преобразования
    ui->sliderLabel->setText(QString("%1").arg(value)); //выводим значение слайдера
    if (ui->checkBox->isChecked()) ui->checkBox->click(); //если слайдер поменян в ручном режиме,
    //то снимаем галку с чекбокса
    shiftValue = value; //записываем значение смещенного нуля
    window->drawCurveOnGist(countCurve(3));
}

void Curver::checkboxChanged(int)
{ //слот для чекбокса, который приравнивает значение смещенного нуля к средней яркости
    if (ui->checkBox->isChecked()) //если включили
    {//сначала отключим слежение за слайдером чтобы не было ложных сигналов
        ui->horizontalSlider->setTracking(false); //отключили
        ui->horizontalSlider->setSliderPosition(averageZ()); //присвоили значение средней яркости
        ui->sliderLabel->setText(QString("%1").arg(averageZ())); //сменили текст в лейбле
        shiftValue = averageZ(); //установили значение нуля равное средней яркости
        ui->horizontalSlider->setTracking(true); //снова включаем слежение за слайдером
    }
}


int Curver::gamma(int source, double value)
{ //гамма коррекция одного канала одного пикселя
    double temp = source;
    temp = temp/255;
    temp = qPow(temp, value);
    temp = temp * 255;
    return temp;

}


void Curver::doGamma(int x, int y)
{ //гамма коррекция одного пикселя
    QColor pixel = image.pixel(x, y); //получаем пиксель
    pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),gamma(pixel.lightness(),gammaValue));
    result.setPixel(x,y,pixel.rgb()); //сохраняем результат
}

int Curver::solarise(int source, double gamma)
{ //соляризация одного канала одного пикселя
    double relative = source; //делаем так же по формуле
    relative = relative/255;
    return (255 * (1 - qPow((2*relative - 1),gamma)));
}

void Curver::doSolar(int x, int y)
{ //соляризация одного пикселя
    QColor pixel = image.pixel(x, y); //получаем пиксель
    pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),solarise(pixel.lightness(),solValue));
    result.setPixel(x,y,pixel.rgb());//сохраняем результат
}

void Curver::newCycle(int mode)
{//функция, применяющая фильтр к каждому пикселю изображения
    window->setMaxProgress(image.height()-1); //будем обновлять прогрес бар после каждой строки
    //qDebug() << "AverZ = " << averZ;
    for (int y = 0; y < image.height(); y++)
    { //для каждой строки
        window->setProgress(y); //обновляем прогрес бар
        for (int x = 0; x < image.width(); x++)
        {//для каждого пикселя
           if (mode == 1) doGamma(x,y); //обрабатываем отдельный пиксель
           if (mode == 2) doSolar(x,y);
           if (mode == 3) doShifted(x,y, shiftValue);
        }
    }
    window->receiveResult(result);
    //showResult(); //показываем результат
}

int Curver::averageZ()
{//функция возвращает среднюю яркость картинки
    int averSum = 0;
    QList<int> pixels;
    for (int x = 0; x < image.width(); x++)
        for (int y = 0; y < image.height(); y++)
        { //запоминаем пиксели
            QColor pixel = image.pixel(x,y);
            pixels.append(pixel.lightness());
        }
    for (int y = 0; y < pixels.size(); y++)
    { // прибавляем к сумме яркость каждого последующего пикселя
        averSum = averSum + pixels[y];
    }
    return averSum / pixels.size() ; //и делим на их количество
}

void Curver::doShifted(int x, int y, int aver)
{ //степенное преобразование одного пикселя
    QColor pixel = image.pixel(x, y); //выполняется аналогично
    pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),shiftedZ(pixel.lightness(), aver,shiftPower));
    result.setPixel(x,y,pixel.rgb());
}

int Curver::shiftedZ(int source, int aver, double powa)
{//степенное преобразование одного канала одного пикселя
    int Zav = aver;
    double pow = 0;
    if (correct==true) pow = powa; //переключатель между режимами
    if (correct==false) pow = 1/powa; //n<1 и n>1

    if (source > Zav) //а тут формула, которую я вывел. Подробнее описана в отчете.
        return Zav +  ((255 - Zav) / qPow(255 - Zav,pow))*(qPow(source - Zav, pow));
    if (source < Zav)
        return Zav +  (Zav / qPow(Zav, pow))*(-qPow(Zav - source, pow));
    else
        return Zav;

}
