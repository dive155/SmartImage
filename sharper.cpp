#include "sharper.h"
#include "ui_sharper.h"

Sharper::Sharper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sharper)
{
    ui->setupUi(this);


    connect(ui->doubleSpinBox_5, SIGNAL(valueChanged(double)),
            this, SLOT(nChanged(double)));
    connect(ui->contBut, SIGNAL(clicked()),
            this, SLOT(contrastSlot()));
    connect(ui->contrastArea, SIGNAL(valueChanged(int)),
            this, SLOT(contAreaChanged(int)));
    connect(ui->uMaskBut, SIGNAL(clicked()),
            this, SLOT(uncertainSlot()));
    connect(ui->uSpin, SIGNAL(valueChanged(int)),
            this, SLOT(kgChanged(int)));
    connect(ui->slider, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged(int)));
    connect(ui->kgSpinbox, SIGNAL(valueChanged(int)),
            this, SLOT(realKgChanged(int)));
    connect(ui->simpleSharpButton, SIGNAL(clicked()),
            this, SLOT(simpleSharp()));
    connect(ui->sRGButton, SIGNAL(clicked()),
            this, SLOT(simpleSharpRGB()));
    connect(ui->gausSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setGausPower(int)));
    connect(ui->sigmaSpin, SIGNAL(valueChanged(double)),
            this, SLOT(setGausSigma(double)));


    kg=1;
    contArea = 2;
    realKg=1;
    gausPower = 1;
    gausSigma = 1;

}

Sharper::~Sharper()
{
    delete ui;
}

void Sharper::setSource(MainWindow *source)
{
    window = source;
}

void Sharper::setImage(QImage picture)
{
    image = picture;
    result = image;
}

void Sharper::kgChanged(int spin)
{
    kg = spin;
}


void Sharper::uncertainSlot()
{
    uncertainMask(&image,2,1);
    window->receiveResult(result);
}

void Sharper::simpleSharp()
{
    uncertainMask(&image,1, 1);
    window->receiveResult(result);
}

void Sharper::simpleSharpRGB()
{
    uncertainMask(&image,1, 2);
    window->receiveResult(result);
}

void Sharper::setGausPower(int spin)
{
    gausPower=spin;
}

void Sharper::setGausSigma(double spin)
{
    gausSigma = spin;
}



QList<int> Sharper::vicinity(QImage *picture, int power, int curx, int cury, int chanel)
{ //функция возвращает окрестность заданного пикселя
    QList<int> proxArray;
    for (int y = cury - power; y <= cury+ power; y++)
        for (int x = curx - power; x <= curx+ power; x++)
        { //проверяем окрестные пиксели
            if ((0 <= x && x <= picture->width()-1) && (0 <= y && y <= picture->height()-1))
            { //запоминаем их цвета
                QColor pixel = picture->pixel(x, y);
                if (chanel == 1)
                    proxArray.append(pixel.red());
                if (chanel == 2)
                    proxArray.append(pixel.green());
                if (chanel == 3)
                    proxArray.append(pixel.blue());
                if (chanel ==4)
                    proxArray.append(pixel.lightness());
            }
            else { //если мы за пределами изображения, то запоминаем чёрный цвет
                proxArray.append(0);
            }
        }
    return proxArray;
}

int Sharper::listAverage(QList<int> theArray)
{ //функция считает среднее арифметическое значение списка
    //qDebug() << "listaverage";
    int aver = 0;
    for (int i = 0; i <= theArray.size()-1; i++)
    {
        aver = aver + theArray[i];
    }
    aver = aver / theArray.size(); // среднее значение равно сумма значений/кол-во значений
    return aver;
}

double Sharper::listDisp(QList<int> theArray, int mu)
{ //функция считает дисперсию значений списка
    //qDebug() << "listdisp";
    double aver = 0; //counting color value
    for (int i = 0; i <= theArray.size()-1; i++)
    {
        aver = aver + ((theArray[i]-mu)*(theArray[i]-mu));
    }
    aver = aver / theArray.size();
    return sqrt(aver);
}


QList<qreal> Sharper::genGauss(int power, qreal sigma)
{
    QList<qreal>ohNo;
    for (int i = 1; i<=power*2+1; i++)
    {
        for (int j = 1; j<=power*2+1; j++)
        {

            ohNo.append( (1.0/sqrt(2.0*3.14159*sigma*sigma)) * qExp( -(qPow((j-power - 1.0), 2) + qPow((i-power - 1.0), 2))/(2.0*sigma*sigma) ));
        }
    }
    qreal sum = 0;
    for (int i = 0; i<ohNo.size(); i++)
        sum = sum+ohNo[i];
    sum =1/sum;
    for (int i = 0; i<ohNo.size(); i++)
        ohNo[i]=ohNo[i]*sum;
    return ohNo;
}

int Sharper::applyKernel(QList<int> area, QList<qreal>  kernel)
{ //функция, накладывающая маски
    int sum = 0;
    for (int i = 0; i < area.size(); i++)
    { //умножаем элемент окрестности на соответствующий элемент маски и
        sum = sum + area[i]*kernel[i]; //считаем сумму всех таких произведений
    }

    return sum ;// 16; //возвращаем Z
}


QColor Sharper::uncertainColor(QImage *picture, int mode, int tempKg, QColor sourcePixel, QList<qreal>gaussian, int x, int y, int hsl)
{
    QColor pixel;
    int value = 0;
    if (hsl == 1)
    {
        QList<int> local = vicinity(picture, gausPower, x, y, 4); //получаем окрестность
        int gauss = applyKernel(local, gaussian); //применяем фильтр гаусса к окрестности
        if (mode == 2) tempKg=countKg(vicinity(picture, kg, x, y, 4));
        int value = gauss + tempKg*(sourcePixel.lightness()-gauss); //считаем новую яркость
        if (value > 255) value = 255;
        if (value < 0) value = 0;
        pixel.setHsl(sourcePixel.hslHue(),sourcePixel.hslSaturation(),value); //задаем новую яркость*/
        return pixel;
    }

    QList<int> local = vicinity(picture, gausPower, x, y, 1); //получаем окрестность
    int gauss = applyKernel(local, gaussian); //применяем фильтр гаусса к окрестности
    if (mode == 2) tempKg=countKg(vicinity(picture, kg, x, y, 1));
    value = gauss + tempKg*(sourcePixel.red()-gauss); //считаем новую яркость
    if (value > 255) value = 255;
    if (value < 0) value = 0;
    pixel.setRed(value);

    local = vicinity(picture, gausPower, x, y, 2); //получаем окрестность
    gauss = applyKernel(local, gaussian); //применяем фильтр гаусса к окрестности
    if (mode == 2) tempKg=countKg(vicinity(picture, kg, x, y, 2));
    value = gauss + tempKg*(sourcePixel.green()-gauss); //считаем новую яркость
    if (value > 255) value = 255;
    if (value < 0) value = 0;
    pixel.setGreen(value);

    local = vicinity(picture, gausPower, x, y, 3); //получаем окрестность
    gauss = applyKernel(local, gaussian); //применяем фильтр гаусса к окрестности
    if (mode == 2) tempKg=countKg(vicinity(picture, kg, x, y, 3));
    value = gauss + tempKg*(sourcePixel.blue()-gauss); //считаем новую яркость
    if (value > 255) value = 255;
    if (value < 0) value = 0;
    pixel.setBlue(value);


    return pixel;
}

void Sharper::uncertainMask(QImage *picture, int mode, int hsl)
{ //нечеткое маскирование
    int tempKg = realKg;
    QList<qreal> gaussian;
    gaussian = genGauss(gausPower,gausSigma);
    window->setMaxProgress(picture->width()-1);
    for (int x = 0; x<picture->width();x++)
    { //обрабатываем всю картинку
        window->setProgress(x);
        for (int y = 0; y<picture->height();y++)
        {
            QColor pixel = uncertainColor(picture, mode, tempKg, picture->pixel(x,y),gaussian,x,y,hsl);
           // pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),value); //задаем новую яркость
            result.setPixel(x,y,pixel.rgb());
        }
    }
}
double Sharper::countKg(QList<int> proxy)
{ //адаптивный коэффициент
    qreal disp = listDisp(proxy,listAverage(proxy));
    if (abs(disp)<0.1) return kn;
    return kn / listDisp(proxy,listAverage(proxy));
}


void Sharper::sliderChanged(int value)
{//слот слайдера для коэффициента фильтра
    kn = value;
}

void Sharper::localContrast(QImage *picture)
{ //фильтрация с учётом локальной контрастности
    window->setMaxProgress(picture->width()-1);
    for (int x = 0; x<picture->width();x++)
    {
        window->setProgress(x);
        for (int y = 0; y<picture->height();y++)
        {
            QList<int> local = vicinity(picture, contArea, x, y, 4); //получаем локальную окрестность
            QColor pixel = picture->pixel(x, y); //делаем пиксель
            int value = contrastPixel(pixel.lightness(), listAverage(local)) ; //считаем фильтр
            if (value > 255) value = 255;
            if (value < 0) value = 0;
            //qDebug() << gauss << " + " << kg << " * (" << pixel.lightness() << " - " <<  gauss << ") = " << value;
            pixel.setHsl(pixel.hslHue(),pixel.hslSaturation(),value);
            result.setPixel(x,y,pixel.rgb()); //записываем
        }
    }
}

int Sharper::contrastPixel(int source, int proxBright)
{ //считаем выходное значение одного пикселя
   // qDebug() << "was " << source << proxBright;
    double kostil = source - proxBright;
    double Cz = qAbs(kostil)/(source+proxBright);
   // qDebug () << "cz" << Cz;
    if (source <= proxBright)
    {
       // qDebug() << "now " << 1.0 - qPow(Cz, N) ;
        return proxBright * ( (1.0 - qPow(Cz, N)) / (1.0 + qPow(Cz, N)) );
    } //всё по формуле считаем в этой функции
    else
    {
        //qDebug() << "now " << (1.0 + qPow(Cz, N)) / (1.0 - qPow(Cz, N));
        return proxBright * ( (1.0 + qPow(Cz, N)) / (1.0 - qPow(Cz, N)) );
    }
}


void Sharper::nChanged(double spin)
{ //слот смнился N
    N = spin;
}

void Sharper::contrastSlot()
{ //вызвать функцию фильтрации
    localContrast(&image);
    window->receiveResult(result);
}

void Sharper::contAreaChanged(int spin)
{ //сменить размер окрестности
    contArea = spin;
}

void Sharper::realKgChanged(int spin)
{
    realKg=spin;
}
