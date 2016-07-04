#include "simpleaver.h"
#include "ui_simpleaver.h"

SimpleAver::SimpleAver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleAver)
{
    ui->setupUi(this);
    connect(ui->pushButton_4, SIGNAL(clicked()),
            this, SLOT(medFilter()));
    connect(ui->pushButton_5, SIGNAL(clicked()),
            this, SLOT(rangFilter()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkSpin(int)));
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)),
            this, SLOT(checkDisplacement(int)));
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)),
            this, SLOT(checkK()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkK()));
    connect(ui->pushButton_6, SIGNAL(clicked()),
            this, SLOT(arAvFilter())); //связываем кнопки с функциями
    connect(ui->pushButtonGarm1, SIGNAL(clicked()),
            this, SLOT(sredneFilter()));
    connect(ui->pushButtonGarm2, SIGNAL(clicked()),
            this, SLOT(contrFilter()));
    connect(ui->spinBoxGarm, SIGNAL(valueChanged(int)),
            this, SLOT(checkdegree(int)));
    connect(ui->sigmaSpin, SIGNAL(valueChanged(double)),
            this, SLOT(setGausSigma(double)));
    connect(ui->gausBut, SIGNAL(clicked()),
            this, SLOT(gausFilter()));

    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(checkCut(int)));
    connect(ui->cutAverBut, SIGNAL(clicked()),
            this, SLOT(cutFilter()));
    connect(ui->cuvBut, SIGNAL(clicked()),
            this, SLOT(cuvahFilter()));
    strength = 1;
    sigma = 0.5;
    cutValue = 0;
    degree=0;
}

SimpleAver::~SimpleAver()
{
    delete ui;
}

void SimpleAver::setSource(MainWindow *source)
{
    window = source;
}

void SimpleAver::setImage(QImage picture)
{
    image = picture;
    result = image;
}


void SimpleAver::checkSpin(int spin)
{ //this set's up area size
    strength = spin;
    ui->spinBox_2->setMaximum(strength*strength);
    ui->horizontalSlider->setMaximum(((2*strength +1)*(2*strength +1)-1)/2);
} //the size limit for the "offset" spinbox is area size squared

void SimpleAver::checkDisplacement(int spin)
{ //this is actually offset, but i call it displacement
    displacement = spin;
}

void SimpleAver::checkK()
{ //this calculates K value so a user knows what the K would be with given settings
   double k = (0.5/(strength*strength))* ui->spinBox_2->value() + 0.5 ;
   ui->label_5->setText(QString("K = %1").arg(k));
   //k = 0.5 means we take the central value (aka median filter)
   //k = 1 means we take a min/max value
}

void SimpleAver::medFilter()
{ //let's apply median filter
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    {
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        {
            xRang(x,y,strength);
        }//strength is the are size
    }
    window->receiveResult(result);
}

void SimpleAver::rangFilter()
{ //let's apply rang filter
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    {
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        {
            Rangovy(x,y,strength,displacement);
        }//displacement is offset
    }
    window->receiveResult(result);
}

void SimpleAver::arAvFilter()
{ //фильтр арифметическое среднее
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++) //для каждой строки
    {
        window->setProgress(y); //увеличиваем значение на прогрес баре
        for (int x = 0; x < image.width(); x++)
        { //для каждого столбца
            arAv(x,y,strength, strength); //применяем фильтр арифметического среднего
        }
    }
    window->receiveResult(result);
}


void SimpleAver::xRang(int curx, int cury, int power)
{ //this does median filter
    QList<int> proxArrayR; //arrays of neighbout pixels
    QList<int> proxArrayG; //of each color
    QList<int> proxArrayB;
    for (int y = cury - power; y <= cury+ power; y++)
        for (int x = curx - power; x <= curx+ power; x++)
        { //let's check all nearby pixels
            if ((0 <= x && x <= image.width()-1) && (0 <= y && y <= image.height()-1))
            { //if we're not outside of the image, let's write down color values
                QColor pixel = image.pixel(x, y);
                proxArrayR.append(pixel.red());
                proxArrayG.append(pixel.green());
                proxArrayB.append(pixel.blue());
            }
            else { //if there aint no pixel, we use black
                proxArrayR.append(0);
                proxArrayG.append(0);
                proxArrayB.append(0);
            }
        }
    qSort(proxArrayR); //now let's sort these thingies
    qSort(proxArrayG);
    qSort(proxArrayB);
    QColor pixel = (QColor(0, 0, 0, 255)); //makin a pixel
    int powercf = proxArrayR.size()/2; //for simplicity. powercf is a middle element
    pixel.setRed(proxArrayR[powercf]);
    pixel.setGreen(proxArrayG[powercf]);
    pixel.setBlue(proxArrayB[powercf]); //setting new colors
    if ((0 <= curx && curx <= image.width()-1) && (0 <= cury && cury <= image.height()-1))
        result.setPixel(curx, cury, pixel.rgb()); //writing dem into a result image
}   //if we're not outside of borders

void SimpleAver::Rangovy(int curx, int cury, int power, int k)
{ //this does rang filter
    QList<int> proxArrayR; //arrays of neighbout pixels
    QList<int> proxArrayG;
    QList<int> proxArrayB;
    for (int y = cury - power; y <= cury+ power; y++)
        for (int x = curx - power; x <= curx+ power; x++)
        { //let's check all nearby pixels
            if ((0 <= x && x <= image.width()-1) && (0 <= y && y <= image.height()-1))
            { //if we're not outside of the image, let's write down their colors
                QColor pixel = image.pixel(x, y);
                proxArrayR.append(pixel.red());
                proxArrayG.append(pixel.green());
                proxArrayB.append(pixel.blue());
            }
            else { //if there aint no pixel, we use black
                proxArrayR.append(0);
                proxArrayG.append(0);
                proxArrayB.append(0);
            }
        }
    qSort(proxArrayR); //now let's sort these thingies
    qSort(proxArrayG);
    qSort(proxArrayB);
    QColor pixel = (QColor(0, 0, 0, 255)); //makin a pixel

    int powercf = proxArrayR.size()/2;

    int zk = 0; int zl = 0; int z = 0; //let's choose between zk and zl
    zk = proxArrayR[powercf-k]; //they're both k positions away from the middle
    zl = proxArrayR[powercf+k];
    z = proxArrayR[powercf];
    if (qAbs(zk - z) >= qAbs(zl - z))// if |zk-z| >= |zl - z| we choose zl
        pixel.setRed(zl);
    if (qAbs(zk - z) < qAbs(zl - z)) //otherwise we pick zk
        pixel.setRed(zk);

    zk = 0; zl = 0; z = 0; //let's choose between zk and zl
    zk = proxArrayG[powercf-k]; //they're both k positions away from the middle
    zl = proxArrayG[powercf+k];
    z = proxArrayG[powercf];
    if (qAbs(zk - z) >= qAbs(zl - z)) // if |zk-z| >= |zl - z| we choose zl
        pixel.setGreen(zl);
    if (qAbs(zk - z) < qAbs(zl - z)) //otherwise we pick zk
        pixel.setGreen(zk);

    zk = 0; zl = 0; z = 0; //let's choose between zk and zl
    zk = proxArrayB[powercf-k]; //they're both k positions away from the middle
    zl = proxArrayB[powercf+k];
    z = proxArrayB[powercf];
    if (qAbs(zk - z) >= qAbs(zl - z))// if |zk-z| >= |zl - z| we choose zl
        pixel.setBlue(zl);
    if (qAbs(zk - z) < qAbs(zl - z)) //otherwise we pick zk
        pixel.setBlue(zk);

    if ((0 <= curx && curx <= image.width()-1) && (0 <= cury && cury <= image.height()-1))
        result.setPixel(curx, cury, pixel.rgb()); //writing dem into a result image
}

void SimpleAver::arAv(int curx, int cury, int powerX, int powerY)
{ //Arithmetic Average для одного пикселя
    QList<int> proxArrayR; //массивы пикселей окрестности точки
    QList<int> proxArrayG; //для трёх цветов (red, green, blue)
    QList<int> proxArrayB;
    for (int y = cury - powerY; y <= cury+ powerY; y++)
        for (int x = curx - powerX; x <= curx+ powerX; x++)
        { //проверяем окрестные пиксели
            if ((0 <= x && x <= image.width()-1) && (0 <= y && y <= image.height()-1))
            { //запоминаем их цвета
                QColor pixel = image.pixel(x, y);
                proxArrayR.append(pixel.red());
                proxArrayG.append(pixel.green());
                proxArrayB.append(pixel.blue());
            }
            else { //если мы за пределами изображения, то запоминаем чёрный цвет
                proxArrayR.append(0);
                proxArrayG.append(0);
                proxArrayB.append(0);
            }
        }
    QColor pixel = (QColor(0, 0, 0, 255)); //создаём чёрный пиксель

    pixel.setRed(listAverage(proxArrayR)); //устанавливаем значение его цветов равным
    pixel.setGreen(listAverage(proxArrayG)); //среднему арифметическому массива
    pixel.setBlue(listAverage(proxArrayB)); //ближайших пикселей

    if ((0 <= curx && curx <= image.width()-1) && (0 <= cury && cury <= image.height()-1))
        result.setPixel(curx, cury, pixel.rgb()); //записываем пиксель в результирующую картинку
}

int SimpleAver::listAverage(QList<int> theArray)
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

/*-----------------------------------------------------------------------*/

void SimpleAver::checkSpinD(int spin)
{ //записываем радиус окрестности
    strength = spin;
}

void SimpleAver::checkdegree(int spin)
{ //записываем порядок контргармонического фильтра
    degree = spin;
}


void SimpleAver::sredneFilter()
{
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    { //для каждой строки картинки
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        { //для каждого столбца картинки
            doSredne(x,y,strength); //применяем среднегармонический фильтр
        }
    }
    window->receiveResult(result);
}

void SimpleAver::contrFilter()
{
    window->setMaxProgress(image.height()-1);
    for (int y = 0; y < image.height(); y++)
    {//для каждой строки картинки
        window->setProgress(y);
        for (int x = 0; x < image.width(); x++)
        { //для каждого столбца картинки
            doContr(x,y,strength,degree); //применяем контргармонический фильтр
        }
    }
    window->receiveResult(result);
}

QList<int> SimpleAver::vicinity(QImage *picture, int power, int curx, int cury, int chanel)
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

void SimpleAver::doSredne(int curx, int cury, int power)
{ //среднегармонический фильр для одного пикселя
    QList<int> proxArrayR = vicinity(&image, power, curx, cury, 1); //получаем окрестность (3 цветовых канала)
    QList<int> proxArrayG = vicinity(&image, power, curx, cury, 2);
    QList<int> proxArrayB = vicinity(&image, power, curx, cury, 3);
    QColor pixel = (QColor(sredne(proxArrayR), sredne(proxArrayG), sredne(proxArrayB), 255)); //ищем новый цвет
    result.setPixel(curx, cury, pixel.rgb()); //записываем пиксель в результирующую картинку
}

void SimpleAver::doContr(int curx, int cury, int power, int rang)
{ //контргармонический фильр для одного пикселя
    QList<int> proxArrayR = vicinity(&image, power, curx, cury, 1); //получаем окрестность (3 цветовых канала)
    QList<int> proxArrayG = vicinity(&image, power, curx, cury, 2);
    QList<int> proxArrayB = vicinity(&image, power, curx, cury, 3);
    QColor pixel = (QColor(contr(proxArrayR, rang), contr(proxArrayG, rang), contr(proxArrayB, rang), 255)); //ищем новый цвет
    result.setPixel(curx, cury, pixel.rgb()); //записываем пиксель в результирующую картинку
}

qreal SimpleAver::sredne(QList<int> src)
{ //функция возвращает значение среднегармонического подсчета массива
    qreal sum = 0; //сумма
    qreal temp = src.size(); //размер массива равен m*n
    for (int i = 0; i<src.size(); i++)
    { //считаем сумму по формуле
        if (src[i]==0) src[i]=1; //это чтобы не делить на 0
        sum = sum + 1.0/src[i];
    }
    sum = temp/sum; //считаем получившуюся яркость
    return sum;
}

qreal SimpleAver::contr(QList<int> src, int rang)
{ //функция возвращает значение контргармонического подсчета массива
    qreal sumH = 0; //числитель
    qreal sumL = 0; //знаменатель
    for (int i = 0; i<src.size(); i++)
    {
        if (src[i]==0) src[i]=1; //это чтобы не делить на 0
        sumH = sumH + qPow(src[i], rang+1); //считаем числитель
        sumL = sumL + qPow(src[i], rang); //считаем знаменатель
    }
    sumH = sumH/sumL; //считаем результат по формуле
    return sumH;
}

QList<qreal> SimpleAver::genGauss(int power, qreal sigma)
{ //генератор маски по Гауссу
    QList<qreal>ohNo;
    for (int i = 1; i<=power*2+1; i++)
    {
        for (int j = 1; j<=power*2+1; j++)
        { //считаем по формуле

            ohNo.append( (1.0/sqrt(2.0*3.14159*sigma*sigma)) * qExp( -(qPow((j-power - 1.0), 2) + qPow((i-power - 1.0), 2))/(2.0*sigma*sigma) ));
        }
    }
    qreal sum = 0;
    for (int i = 0; i<ohNo.size(); i++)
        sum = sum+ohNo[i];
    sum =1/sum; //нормализуем
    for (int i = 0; i<ohNo.size(); i++)
        ohNo[i]=ohNo[i]*sum;
    return ohNo;
}

int SimpleAver::applyKernel(QList<int> area, QList<qreal>  kernel)
{ //функция, накладывающая маски
    QList<qreal> reals;
    for (int i = 0; i<area.size(); i++)
        reals << area[i];
    qreal sum = 0;
    for (int i = 0; i < area.size(); i++)
    { //умножаем элемент окрестности на соответствующий элемент маски и
        sum = sum + reals[i]*kernel[i]; //считаем сумму всех таких произведений
    }

    return sum ;// 16; //возвращаем Z
}

void SimpleAver::gausFilter()
{
    QList<qreal> gaussian = genGauss(strength, sigma);
    window->setMaxProgress(image.width()-1);
    for (int x = 0; x<image.width(); x++)
    {
        window->setProgress(x);
        for (int y = 0; y<image.height(); y++)
        {
            QColor pixel = image.pixel(x, y);
            pixel.setRed(applyKernel(vicinity(&image,strength,x,y,1),gaussian));
            pixel.setGreen(applyKernel(vicinity(&image,strength,x,y,2),gaussian));
            pixel.setBlue(applyKernel(vicinity(&image,strength,x,y,3),gaussian));
            result.setPixel(x,y,pixel.rgb());
        }
    }
    window->receiveResult(result);
}

void SimpleAver::setGausSigma(double spin)
{
    sigma = spin;
}

//--------------------------------------------------------------------------------------



QList<int> SimpleAver::cutList(QList<int> sourceList, int cutValue)
{ //усечение массива пикселей
    QList<int> tempList;
    for (int i = cutValue; i < sourceList.size()-cutValue; i++)
        tempList.append(sourceList[i]);
    return tempList;

}

void SimpleAver::cutFilter()
{ //фильтр усеченного среднего
    window->setMaxProgress(image.width()-1);
    QList<int> proxR, proxG, proxB;
    for (int x = 0; x<image.width(); x++)
    {
        window->setProgress(x);
        for (int y = 0; y<image.height(); y++)
        {
            proxR = vicinity(&image,strength,x,y,1);
            qSort(proxR); //получаем окрестности
            proxG = vicinity(&image,strength,x,y,2);
            qSort(proxG);
            proxB = vicinity(&image,strength,x,y,3);
            qSort(proxB);
            QColor pixel;
            //устанавливаем цвет равным среднему арефметическому от усеченного массива:
            pixel.setRed(listAverage(cutList(proxR, cutValue)));
            pixel.setGreen(listAverage(cutList(proxG, cutValue)));
            pixel.setBlue(listAverage(cutList(proxB, cutValue)));
            result.setPixel(x,y,pixel.rgb()); //записываем результат
        }
    }
    window->receiveResult(result); //показываем результат
}

double SimpleAver::listDisp(QList<int> theArray, int mu)
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

void SimpleAver::cuvahFilter()
{ //фильтр кувахары
    window->setMaxProgress(image.width()-1);
    for (int x = 0; x<image.width(); x++)
    {
        window->setProgress(x);
        for (int y = 0; y<image.height(); y++)
        {
            QColor pixel;
            //присваиваем пикселю результат обработки окрестности фильтром Кувахары
            pixel.setRed(doCuvah(vicinity(&image,strength,x,y,1)));
            pixel.setGreen(doCuvah(vicinity(&image,strength,x,y,2)));
            pixel.setBlue(doCuvah(vicinity(&image,strength,x,y,3)));
            result.setPixel(x,y,pixel.rgb()); //записываем результат
        }
    }
    window->receiveResult(result); //показываем результат
}

int SimpleAver::doCuvah(QList<int> sourceList)
{ //возвращаем результат обработки массива фильтром Кувахары
    QList<int> quad1,quad2,quad3,quad4;
    int dr = 2*strength;
    int dro = 2*strength+1;
    //получаем 1й квадрант:
    for (int j = 0; j<strength; j++)
        for (int i = 0; i<strength; i++)
            quad1 << sourceList[j*dro + i];
    int aver1 = listAverage(quad1); //получаем среднюю яркость квадранта
    double disp1 = listDisp(quad1,aver1); //получаем дисперсию квадранта
    //qDebug() << quad1 <<aver1 << disp1;
    //получаем 2й квадрант:
    for (int j = 0; j<strength; j++)
        for (int i = 0; i<strength; i++)
            quad2 << sourceList[j*dro + dr-i];
    int aver2 = listAverage(quad2); //получаем среднюю яркость квадранта
    double disp2 = listDisp(quad2,aver2); //получаем дисперсию квадранта
    //qDebug() << quad2 <<aver2 << disp2;
    //получаем 3й квадрант:
    for (int j = 0; j<strength; j++)
        for (int i = 0; i<strength; i++)
            quad3 << sourceList[(dr-j)*dro + i];
    int aver3 = listAverage(quad3); //получаем среднюю яркость квадранта
    double disp3 = listDisp(quad3,aver3); //получаем дисперсию квадранта
    //qDebug() << quad3 <<aver3 << disp3;
    //получаем 4й квадрант:
    for (int j = 0; j<strength; j++)
        for (int i = 0; i<strength; i++)
            quad4 << sourceList[(dr-j)*dro + dr-i];
    int aver4 = listAverage(quad4); //получаем среднюю яркость квадранта
    double disp4 = listDisp(quad4,aver4); //получаем дисперсию квадранта
    //qDebug() << quad4 <<aver4 << disp4;

    //ищем квадрант с минимальной дисперсией:
    double minDisp = disp1;
    int min=1;
    if (disp2<minDisp)
    {
        minDisp = disp2;
        min = 2;
    }
    if (disp3<minDisp)
    {
        minDisp = disp3;
        min = 3;
    }
    if (disp4<minDisp)
    {
        minDisp = disp4;
        min = 4;
    }

    //возвращаем среднюю яркость квадранта с минимальной дисперсией:
    if (min == 1)
        return aver1;
    if (min == 2)
        return aver2;
    if (min == 3)
        return aver3;
    else
        return aver4;

}

void SimpleAver::checkCut(int spin)
{
    cutValue = spin;
    ui->cut_Label->setText(QString("Truncation: %1").arg(spin));
}
