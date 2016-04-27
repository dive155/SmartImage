#include "edger.h"
#include "ui_edger.h"

Edger::Edger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Edger)
{
    ui->setupUi(this);

    filter = 1; //исходный фильтр - Уоллес

    connect(ui->but1, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    connect(ui->but2, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    connect(ui->but3, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    connect(ui->but4, SIGNAL(toggled(bool)),
            this, SLOT(radio()));
    //связываем сигналы со слотами (кнопки с функциями):
    connect(ui->filterButton, SIGNAL(clicked()),
            this, SLOT(doFilter()));
    //задаем маски наших фильтров
    QList<int> ker1; //фильтр Кирша использует 8 масок:
    ker1 << 5 <<  5 << 5
         <<-3 <<  0 << -3
         <<-3 <<-3  << -3;
    QList<int> ker2;
    ker2 << 5 <<  5 << -3
         <<5  <<  0 << -3
         <<-3 <<-3  << -3;
    QList<int> ker3;
    ker3 << 5 << -3 << -3
         <<5  <<  0 << -3
         <<5  <<-3  << -3;
    QList<int> ker4;
    ker4 << -3<< -3 << -3
         <<5  <<  0 << -3
         <<5  << 5  << -3;
    QList<int> ker5;
    ker5 << -3<< -3 << -3
         <<-3 <<  0 << -3
         <<5  << 5  << 5;
    QList<int> ker6;
    ker6 << -3<< -3 << -3
         <<-3 <<  0 << 5
         <<-3 << 5  << 5;
    QList<int> ker7;
    ker7 << -3<< -3 << 5
         <<-3 <<  0 << 5
         <<-3 << -3 << 5;
    QList<int> ker8;
    ker8 << -3<< 5  << 5
         <<-3 <<  0 << 5
         <<-3 << -3 << -3;
    kernels << ker1 << ker2 << ker3 << ker4 << ker5 << ker6 << ker7 << ker8 ;

    //фильтрые Собеля и Превитта - по две маски
    sobelx << -1<< 0  << 1
           <<-2 <<  0 << 2
           <<-1 << 0  << 1;
    sobely << -1<< -2 << -1
           <<0  <<  0 << 0
           <<1  << 2   << 1;
    prewitx << -1<< 0  << 1
            <<-2 <<  0 << 2
            <<-1 << 0  << 1;
    prewity << -1<< -2  << -1
            <<0  <<  0  << 0
            <<1  << 2   << 1;
}

Edger::~Edger()
{
    delete ui;
}

void Edger::setSource(MainWindow *source)
{
    window = source;
}

void Edger::setImage(QImage picture)
{
    image = picture;
    result = image;
}


void Edger::doBNW(QImage &target)
{ //создание чёрно-белой версии картинки
    QImage greyPic; //загатовка под чб картинку
    greyPic = QImage(target); //сначала копируем исходную картинку без изменений

    for (int x = 0; x < target.width(); x++)
        for (int y = 0; y < target.height(); y++) //затем с каждым пикселем делаем следующее:
        {
            QColor pixel = target.pixel(x, y);//получаем цвет текущего пикселя
            //затем по формуле перевода в grayscale считаем яркость
            uint color = pixel.red()*0.3 + pixel.green()*0.59 + pixel.blue()*0.11;
            //так как у нас оттенки серого, RGB должны быть равны между собой
            pixel.setRed(color);
            pixel.setGreen(color);
            pixel.setBlue(color);
            greyPic.setPixel(x, y, pixel.rgb()); //записываем полученный серый цвет в пиксель
        }
    blackNWhite = greyPic; //blackNWhite - глобальная переменная
}

void Edger::doFilter()
{//функция, применяющая фильтр к каждому пикселю изображения
    doBNW(image); //делаем чёрно-белую версию картинки
    //int lcdMax = blackNWhite.height()-1; //lcd индикатору понадобится полное количество шагов цикла
    window->setMaxProgress(blackNWhite.height()-1);
    preCoef.clear(); //очищаем (на всякий случай) массив предварительных значений яркости
    for (int y = 0; y < blackNWhite.height(); y++)
    { //для каждой строки
        window->setProgress(y/2);
        //ui->lcdNumber->display(countLCD(y/2,lcdMax)); //обновляем значение lcd индикатора
        qApp->processEvents(); //обновляем виджет чтобы не казалось что всё зависло
        for (int x = 0; x < blackNWhite.width(); x++)
        {//для каждого пикселя
            filtPixel(x,y, 1); //применяем наш фильтр в режиме 1
        } //режим 1 означает что нормирующий коэффициент ещё не известен. В этой ситуации
    } //мы накладываем фильтр без норм. коэффициента. При этом изменения не отражаются на результирующем
    //изображении - полученные данные будут использованы только для подсчета нормирующего коэффициента
    coef = 255 / maxOfList(preCoef); // считаем нормирующий коэффициент
    for (int y = 0; y < blackNWhite.height(); y++)
    {
        //ui->lcdNumber->display(countLCD(blackNWhite.height()/2 + y/2,lcdMax));
        window->setProgress(blackNWhite.height()/2 + y/2);
        qApp->processEvents();
        for (int x = 0; x < blackNWhite.width(); x++) //применяем наш фильтр в режиме 2
        { //режим 2 означает что нормирующий коэффициент известен, а значит мы можем безопасно
            //сохранить наши изменения в результирующее изображение
            filtPixel(x,y,2);
        }
    }
    window->receiveResult(result);
}


double Edger::maxOfList(QList<int> aList)
{ //функция, находящая максимумальное значение в списке.
  //нужна для подсчета нормирующего коэффициента
    int max = 0;
    for (int i = 0;i < aList.size()-1; i++)
        if (aList[i] > max) max = aList[i];
    return max;
}

int Edger::applyKernel(QList<int> area, QList<int>  kernelx, QList<int>  kernely)
{ //функция, накладывающая маски для осей X и Y
    int sumx = 0;
    for (int i = 0; i < area.size(); i++)
    { //умножаем элемент окрестности на соответствующий элемент маски и
        sumx = sumx + area[i]*kernelx[i]; //считаем сумму всех таких произведений
    }
    int sumy = 0;
    for (int i = 0; i < area.size(); i++)
    {
        sumy = sumy + area[i]*kernely[i];
    }
    return abs(sumx)+abs(sumy); //возвращаем Z = |Zx| + |Zy|
}

int Edger::applyWallace(QList<int> area)
{ /* Функция, применяющая фильтр Уоллеса. Он задается формулой, а не матрицей, однако для упрощения
    вычислений я решил использовать уже имеющуюся у нас окрестность пикселя чтобы брать из неё элементы
    и подставлять их формулой. Окрестность нашего пикселя и массив area соотносятся следующим образом:
                 окрестность пикселя:             area:
      [x-1][y-1]   [ x ][y-1]   [x+1][y-1]     [0] [1] [2]
      [x-1][ y ]   [ x ][ y ]   [x+1][ y ]  =  [3] [4] [5]
      [x-1][y+1]   [ x ][y+1]   [x+1][y+1]     [6] [7] [8]

      формула: z'(x,y) = ln ( z(x,y)/z(x-1,y) * z(x,y)/z(x,y-1) *
       z(x,y)/z(x+1,y) * z(x,y)/z(x,y+1) )
      */
    QList<qreal> reals;
    for (int k = 0; k < 8; k++)
    { //для начала переводи окрестность в real
        reals << area[k];
        if (reals[k] == 0) reals[k] = 1; //и убираем нули чтобы не делить на ноль
    }
    return abs ( qLn ( (reals[4]/reals[3]) * (reals[4]/reals[1]) * (reals[4]/reals[5]) * (reals[4]/reals[7]) ) );
}   //возвращаем значение посчитанное по формуле

int Edger::applyKirschKernel(QList<int> area, QList<QList<int> > kernel)
{ //в случае с фильром Кирша нам нужно сосчитать сумму для каждой из 8 масок
    int max = 0; //и выбрать максимальную из них
    for (int k = 0; k < kernel.size(); k++)
    {
        int sum = 0; //сумма для одной маски
        for (int i = 0; i < area.size(); i++)
        {
            sum = sum + area[i]*kernel[k][i];
        } //считаем эту сумму
        if (sum>max) max = sum; //если она больше имеющегося максимума, запоминаем её
    }
    max = abs(max);
    return max; //возвращаем максимальную сумму
}

void Edger::filtPixel(int curx, int cury, int mode)
{ //функция применения фильтра к одному пикселю
    int power = 1; //радиус окрестности
    QList<int> proxArray; //массивы яркостей окрестных точек
    for (int y = cury - power; y <= cury+ power; y++) //от верхней границы области до нижней
        for (int x = curx - power; x <= curx+ power; x++) //слева направо
        {
            if ((0 <= x && x <= blackNWhite.width()-1) && (0 <= y && y <= blackNWhite.height()-1))
            { //если мы не вылезли за пределы картинки
                QColor pixel = blackNWhite.pixel(x, y); //записываем цвет пикселя
                proxArray.append(pixel.red()); //в массив окрестных пикселей
            }
            else { //если вылезли за пределы картинки, считаем пиксель чёрным
                proxArray.append(0);
            }
        }
    int Sum = 0; //результат применения фильтра на окрестность
    if (filter == 1)
    { //в зависимости от выбранного фильтра, вызываем соответствующий фильтр
         Sum = applyWallace(proxArray);
    }
    if (filter == 2)
    {
         Sum = applyKernel(proxArray, sobelx, sobely);
    }
    if (filter == 3)
    {
         Sum = applyKernel(proxArray, prewitx, prewity);
    }
    if (filter == 4)
    {
         Sum = applyKirschKernel(proxArray, kernels);
    }

    if (mode == 1) //если нормирующий коэффициент еще неизвестен, мы запоминаем
        //яркость пикселя и на этом заканчиваем
        preCoef.append(Sum);
    if (mode == 2)
    { //если нормирующий коэффициент уже известен, мы умножаем значение суммы на него
        Sum = Sum*coef;
        //затем создаём пиксель
        QColor pixel = (QColor(0, 0, 0, 255));
        //присваиваем ему цвет в соответствии с посчитанным значением
        pixel.setRed(Sum);
        pixel.setGreen(Sum);
        pixel.setBlue(Sum);
        //и, на всякий случай проверив что мы ещё в картинке, записываем пиксель в картинку
        if ((0 <= curx && curx <= image.width()-1) && (0 <= cury && cury <= image.height()-1))
            result.setPixel(curx, cury, pixel.rgb());
    }
}


void Edger::radio()
{ //функция срабатывающая при переключении на исходное изображение
    if (ui->but1->isChecked())
    {
        filter = 1;
    }
    if (ui->but2->isChecked())
    {
        filter = 2;
    }
    if (ui->but3->isChecked())
    {
        filter = 3;
    }
    if (ui->but4->isChecked())
    {
        filter = 4;
    }
}
