#include "adaptaver.h"
#include "ui_adaptaver.h"

AdaptAver::AdaptAver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdaptAver)
{
    ui->setupUi(this);

    connect(ui->spinBox_2, SIGNAL(valueChanged(int)),
            this, SLOT(checkVariance(int)));
    connect(ui->pushButton_5, SIGNAL(clicked()),
            this, SLOT(adArAvFilter()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkSpin(int)));

    ui->spinBox_2->setMinimum(50);
    ui->spinBox_2->setMaximum(30000); //ограничиваем значение шума
    ui->spinBox_2->setValue(50);
    ui->spinBox_2->setSingleStep(50);
    dispNoise = 50; //устанавливаем исходное значение дисперсии шума

    connect(ui->pushButton_2, SIGNAL(clicked()),
            this, SLOT(adaptFilter()));
    connect(ui->spinBox_3, SIGNAL(valueChanged(int)),
            this, SLOT(checkLimit(int)));
    limit=1;
    strength = 1;
}

AdaptAver::~AdaptAver()
{
    delete ui;
}


void AdaptAver::setSource(MainWindow *source)
{
    window = source;
}

void AdaptAver::setImage(QImage picture)
{
    image = picture;
    result = image;
}

void AdaptAver::checkSpin(int spin)
{ //qDebug() << "checkspin";
    strength = spin;
} //перевод значения размера окрестности по Х в программу


void AdaptAver::checkVariance(int spin)
{ //функция управляющая спинбоксом настройки дисперсии шума
    if (spin <= 500) //если значение меньше 500
        ui->spinBox_2->setSingleStep(50); //устанавливаем шаг в 50
    if (spin > 500 && spin<600) //если больше 500
    {
        ui->spinBox_2->setValue(1000); //устанавливаем шаг в 500
        ui->spinBox_2->setSingleStep(500);
    }
    dispNoise = spin; //вносим значение в программу
}

void AdaptAver::adArAvFilter()
{ //фильтр адаптивный арифметический
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    {   window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        {
            //yRang(x,y,5);
            adArAv(x,y,strength,strength); //применяем фильтр
        }
    }
    window->receiveResult(result);
    //showImage();
}


int AdaptAver::listAverage(QList<int> theArray)
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

double AdaptAver::listDisp(QList<int> theArray, int mu)
{ //функция считает дисперсию значений списка
    //qDebug() << "listdisp";
    double aver = 0; //counting color value
    for (int i = 0; i <= theArray.size()-1; i++)
    {
        aver = aver + ((theArray[i]-mu)*(theArray[i]-mu));
    }
    aver = aver / theArray.size();
    return aver;
}

void AdaptAver::adArAv(int curx, int cury, int powerX, int powerY)
{ //Adaptive Arithmetic Average
    //qDebug() << "adarav";
    QList<int> proxArrayR; //массивы пикселей окрестности точки
    QList<int> proxArrayG; //для трёх цветов (red, green, blue)
    QList<int> proxArrayB;
    for (int y = cury - powerY; y <= cury+ powerY; y++)
        for (int x = curx - powerX; x <= curx+ powerX; x++)
        {//проверяем окрестные пиксели
            if ((0 <= x && x <= image.width()-1) && (0 <= y && y <= image.height()-1))
            { //записываем значения
                QColor pixel = image.pixel(x, y);
                proxArrayR.append(pixel.red());
                proxArrayG.append(pixel.green());
                proxArrayB.append(pixel.blue());
            }
            else { //если мы за пределами картинки - записываем чёрный цвет
                proxArrayR.append(0);
                proxArrayG.append(0);
                proxArrayB.append(0);
            }
        }


    QColor pixel = image.pixel(curx, cury);//(QColor(0, 0, 0, 255)); //makin a pixel

    int aveRed = listAverage(proxArrayR); //устанавливаем значение его цветов равным
    int aveGreen = listAverage(proxArrayG);//среднему арифметическому массива ближайших пикселей
    int aveBlue = listAverage(proxArrayB);

    double dispRed = listDisp(proxArrayR, aveRed); //считаем дисперсии значений этих массивов
    double dispGreen = listDisp(proxArrayG, aveGreen);
    double dispBlue = listDisp(proxArrayB, aveBlue);


    if (dispNoise > dispRed) //если значение дисперсии шума выше, чем дисперсия массива окрестности
        pixel.setRed(aveRed); //фильтр работает как фильтр арифметического среднего
    if (dispNoise <= dispRed) //иначе значение задается по формуле z' = z - (Dшум/Dz)*(z - u), u - локальное среднее яркости пикселей
        pixel.setRed(pixel.red()- (dispNoise/dispRed)*(pixel.red()-aveRed));

    if (dispNoise > dispGreen)
        pixel.setGreen(aveGreen);
    if (dispNoise <= dispGreen)
        pixel.setGreen(pixel.green()- (dispNoise/dispGreen)*(pixel.green()-aveGreen));

    if (dispNoise > dispBlue)
        pixel.setBlue(aveBlue);
    if (dispNoise <= dispBlue)
        pixel.setBlue(pixel.blue()- (dispNoise/dispBlue)*(pixel.blue()-aveBlue));

    if ((0 <= curx && curx <= image.width()-1) && (0 <= cury && cury <= image.height()-1))
        result.setPixel(curx, cury, pixel.rgb()); //записываем значение в результат
}

/*----------------------------------------------------------------------------------------*/

void AdaptAver::checkLimit(int spin)
{ //записываем верхний предел радиуса окрестности для адаптивного фильтра
    limit = spin;
}

void AdaptAver::adaptFilter()
{
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    {//для каждой строки картинки
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        { //для каждого столбца картинки
            adaptMedian(x,y,limit); //применяем адаптивный медианный фильтр
        }
    }
    window->receiveResult(result);
}


void AdaptAver::adaptMedian(int curx, int cury, int maxR)
{ //адаптивный фильтр для одного пикселя
    QColor pixel = QColor(0, 0, 0, 255);
    for (int i = 1; i <= maxR; i++)
    { //до тех пор пока окрестность не расширится до максимального размера
        QList<int> proxArrayR = vicinity(&image, i, curx, cury, 1); //берем окрестность по красному каналу
        int Zmin = listMin(proxArrayR); //минимальное значение в ней
        int Zmax = listMax(proxArrayR); //максимальное
        int Zmed = median(proxArrayR); //медианное
        int value = proxArrayR[proxArrayR.size()/2]; //значение самого текущего пикселя
        //QDebug () <<"red"<< Zmin << Zmax << Zmed << value;
        if ((Zmin < Zmed) && (Zmed < Zmax))
        { //проверяем первое условие
            if ((Zmin < value) && (value < Zmax))
            { //проверяем второе условие
                pixel.setRed(value); //если выполнено оставляем яркость без изменения
                //QDebug() << "1";
                break;
            }
            else
            {
                pixel.setRed(Zmed); //если нет то возвращаем медианное значение
                //QDebug() << "2";
                break;
            }
        }
        if (i == maxR) //если оба условия не выполнены и мы дошли до конца цикла
            pixel.setRed(value); //оставляем пиксель без изменений
    }
    //далее всё то же самое еще 2 раза для 2х каналов (зеленый и синий)
    for (int i = 1; i <= maxR; i++)
    {
        QList<int> proxArrayG = vicinity(&image, i, curx, cury, 2);
        int Zmin = listMin(proxArrayG);
        int Zmax = listMax(proxArrayG);
        int Zmed = median(proxArrayG);
        int value = proxArrayG[proxArrayG.size()/2];
        //QDebug () <<"green"<< Zmin << Zmax << Zmed << value;
        if ((Zmin < Zmed) && (Zmed < Zmax))
        {
            if ((Zmin < value) && (value < Zmax))
            {
                pixel.setGreen(value);
                //QDebug() << "3";
                break;
            }
            else
            {
                pixel.setGreen(Zmed);
                //QDebug() << "4";
                break;
            }
        }
        if (i == maxR)
            pixel.setGreen(value);
    }
    for (int i = 1; i <= maxR; i++)
    {
        QList<int> proxArrayB = vicinity(&image, i, curx, cury, 3);
        int Zmin = listMin(proxArrayB);
        int Zmax = listMax(proxArrayB);
        int Zmed = median(proxArrayB);
        int value = proxArrayB[proxArrayB.size()/2];
        //QDebug () <<"blue"<< Zmin << Zmax << Zmed << value;
        if ((Zmin < Zmed) && (Zmed < Zmax))
        {
            if ((Zmin < value) && (value < Zmax))
            {
                pixel.setBlue(value);
                //QDebug() << "5";
                break;
            }
            else
            {
                pixel.setBlue(Zmed);
                //QDebug() << "6";
                break;
            }
        }
        if (i == maxR)
            pixel.setBlue(value);
    }
    result.setPixel(curx, cury, pixel.rgb()); //записываем пиксель в результирующую картинку
}

QList<int> AdaptAver::vicinity(QImage *picture, int power, int curx, int cury, int chanel)
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

int AdaptAver::median(QList<int> src)
{ //функция возвращает медианное значение массива
    qSort(src);
    return src[(src.size()/2)];
}

int AdaptAver::listMax(QList<int> src)
{ //функция возвращает максимальное значение массива
    int temp = 0;
    for (int i = 1; i<src.size(); i++)
    {
        if (src[i]>temp) temp=src[i];
    }
    return temp;
}

int AdaptAver::listMin(QList<int> src)
{ //функция возвращает минимальное значение массива
    int temp = 255;
    for (int i = 1; i<src.size(); i++)
    {
        if (src[i]<temp) temp=src[i];
    }
    return temp;
}
