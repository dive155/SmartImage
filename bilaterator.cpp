#include "bilaterator.h"
#include "ui_bilaterator.h"

Bilaterator::Bilaterator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Bilaterator)
{
    ui->setupUi(this);

    connect(ui->sigmaSpin, SIGNAL(valueChanged(double)),
            this, SLOT(setGausSigma(double)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkSpin(int)));
    connect(ui->sensSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(checkVertSigma(int)));
    connect(ui->bilBut, SIGNAL(clicked()),
            this, SLOT(applyFilter()));

    sigma = 1;
    strength = 1;
    vertSigma = 30;
    sigmaCoef = 1.0/(qExp(-(0)/(2*vertSigma*vertSigma))/(vertSigma*qSqrt(2*3.14)));
    vertSigma = 30;
}

qreal Bilaterator::countGaussCoef(int centerValue, int pixValue, qreal coef)
{ //формула меняет коэффициент матрицы Гаусса в зависимости от разницы яркости между
  //соответствующим пикселем и центрельным пикселем окрестности
    int value = abs(centerValue - pixValue);
    return qExp(-(value*value)/(2*vertSigma*vertSigma))/(vertSigma*qSqrt(2*3.14))*coef;
}


Bilaterator::~Bilaterator()
{
    delete ui;
}

void Bilaterator::setSource(MainWindow *source)
{
    window = source;
}

void Bilaterator::setImage(QImage picture)
{
    image = picture;
    result = image;
}

QList<qreal> Bilaterator::genGauss(int power, qreal sigma)
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

int Bilaterator::applyKernel(QList<int> area, QList<qreal>  kernel)
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

QList<int> Bilaterator::vicinity(QImage *picture, int power, int curx, int cury, int chanel)
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

QList<qreal> Bilaterator::modifyGaussian(QList<int> area, QList<qreal> kernel, qreal coef)
{ //функция модифицирует маску Гаусса в зависимости от яркостей окрестных пикселей
    QList<qreal> wegot = kernel;
    int firstIndex = area[area.size()/2];
    for (int i =0; i<kernel.size(); i++)
    { //учитываем разницу яркости между центральным и проверяемым пикселем
        wegot[i]=kernel[i]*countGaussCoef(firstIndex,area[i], coef);
    }

    qreal sum = 0;
    for (int i = 0; i<wegot.size(); i++)
        sum = sum+wegot[i];
    sum =1/sum; //нормализуем
    for (int i = 0; i<wegot.size(); i++)
        wegot[i]=wegot[i]*sum;
    return wegot;

}

void Bilaterator::applyFilter()
{ //применяем фильтр ко всей картинке
    QList<qreal> gaussian = genGauss(strength, sigma);
    window->setMaxProgress(image.width()-1);
    for (int x = 0; x<image.width(); x++)
    {
        window->setProgress(x);
        for (int y = 0; y<image.height(); y++)
        {
            QColor pixel = image.pixel(x, y);
            QList<int> vicinityR = vicinity(&image,strength,x,y,1);
            QList<int> vicinityG = vicinity(&image,strength,x,y,2);
            QList<int> vicinityB = vicinity(&image,strength,x,y,3);
            pixel.setRed(applyKernel(vicinityR,modifyGaussian(vicinityR,gaussian,sigmaCoef)));
            pixel.setGreen(applyKernel(vicinityG,modifyGaussian(vicinityG,gaussian,sigmaCoef)));
            pixel.setBlue(applyKernel(vicinityB,modifyGaussian(vicinityB,gaussian,sigmaCoef)));

            result.setPixel(x,y,pixel.rgb());
        }
    }
    window->receiveResult(result);
}

void Bilaterator::setGausSigma(double spin)
{ //слот для сигмы
    sigma = spin;
}


void Bilaterator::checkSpin(int spin)
{
    strength = spin;
} //перевод значения размера окрестности

void Bilaterator::checkVertSigma(int spin)
{
    vertSigma = spin;
    sigmaCoef = 1.0/(qExp(-(0)/(2*vertSigma*vertSigma))/(vertSigma*qSqrt(2*3.14)));
}
