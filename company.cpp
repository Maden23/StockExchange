#include "company.h"
#include <random>
#include <iostream>
#include <QDebug>
#include <chrono>

using namespace std;

Company::Company()
{
    _name = "DefaultString";
    _available = 10;
    _price = 100;
    _falling = false;
    _initialAvailable = _available;
    _initialPrice = _price;
}
Company::Company(QString name, int price, int available)
{
    (name != "")? _name = name: _name = "DefaultString";
    (available != 0)? _available = available: _available = 10;
    (price != 0)? _price = price: _price = 100;
    _falling = false;
    _initialAvailable = _available;
    _initialPrice = _price;
}

Company::~Company()
{
}

int Company::changePrice()
{
   // можно было использовать rand() , но этот способ дает лучшее распределение по числам
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); //Get current time
    static std::mt19937 generator (seed);
    //static mt19937 generator(randomDevice()); // Генератор случайных чисел.
    static uniform_int_distribution<> distribution(1 , 500); // шанс 1 на 10000 что акция обесценится до 0
    int bancrupt = distribution(generator);

    if (bancrupt != 500)
    {
        // Если на предыдущей итерации цена упала, то изменение в процентах может быть [-20; 10],
        // если выросла, то [-10; 20]
        static std::uniform_int_distribution<> fallingDistr(-20, 10);
        static std::uniform_int_distribution<> risingDistr(-10, 20);

        int percent;
        if (_falling)
        {
            percent = fallingDistr(generator);
        }
        else
        {
            percent = risingDistr(generator);
        }
        _price += _price*percent/100;

        // Обновляем _falling - если x-отрицательный, значит цена упала
        if (percent < 0)
            _falling = true;
        else
            _falling = false;
    }
    else if (bancrupt == 500)
        _price = 0;

    return _price;
}

void Company::setInitialPrice(int price)
{
    if (price > 0)
    {
        _initialPrice = price;
    }
}

void Company::setInitialAvailable(int available)
{
    if (available > 0)
    {
        _initialAvailable = available;
    }
}

void Company::reset()
{
    _available = _initialAvailable;
    _price = _initialPrice;
    _falling = false;
}

void Company::write(ofstream &out)
{
    out << _name.toStdString() << "|"
        << _price << "|"
        << _available << endl;
}

void Company::read(ifstream &in)
{
    in.ignore(); // Удаление символов в буфере потока (переводы строки)
    string name = "";
    getline(in, name, '|');
    setName(QString::fromStdString(name));

    string price, available;
    getline(in, price, '|');
    getline(in, available);

    _price = QString::fromStdString(price).toInt();
    _available = QString::fromStdString(available).toInt();
    _initialPrice = _price;
    _initialAvailable = _available;
}

