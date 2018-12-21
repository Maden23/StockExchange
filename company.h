#ifndef STOCK_H
#define STOCK_H
#include "info.h"
#include <iostream>
using namespace std;

class Company : public Info
{
public:
    Company();
    Company(QString name, int price, int available);
    ~Company();

    // Случайное изменение цены
    int changePrice();

    // Изменение количества акций в наличии
    void changeAvailable(int change) {_available += change;}

    int getPrice() const { return _price; }
    int getAvailable() const { return _available; }

    // Получение и установка начальных параметроы
    int getInitialPrice() const { return _initialPrice; }
    int getInitialAvailable() const { return _initialAvailable; }
    void setInitialPrice(int price);
    void setInitialAvailable(int available);

    // Сброс к начальным параметрам
    virtual void reset();

    virtual info_t defineInfo() const { return IS_COMPANY; }

private:
    int _available;
    int _price;
    bool _falling; // На предыдущей итерации цена упала

    // Переменные для хранения начального состояния
    int _initialAvailable;
    int _initialPrice;

    virtual void write(ofstream &out);
    virtual void read(ifstream &in);
};

#endif // COMPANY_H
