#ifndef HOLDER_H
#define HOLDER_H
#include <map>
#include "info.h"
#include "company.h"
#include <random>
#include <iostream>
#include <QList>

using namespace std;

enum record_t {SOLD, BOUGHT};

// Запись в истории покупок-продаж
struct Record
{
    int day;
    record_t recordType;
    map<Company*,int> stocks;
};

class Holder : public Info
{
public:
    Holder();
    Holder(QString name, int money);
    ~Holder();

    map<Company*,int> getStocks() { return  _stocks; }
    void setStocks(const map<Company*,int> &stocks);

    QList<Record> getHistory() const { return _history; }
    QList<int> getBalance() const { return _balance; }


    virtual map<Company*,int> buy(map<Company*,int> *ratesHistory, int day);
    virtual map<Company*,int> sell(map<Company*,int> *ratesHistory, int day);

    virtual info_t defineInfo() const { return  IS_HOLDER; }
    virtual void reset();

    void setMoney(int money);

protected:
    int _money;
    // Имеющиеся акции
    map<Company*,int> _stocks;
    // История покупок-продаж
    QList<Record> _history;
    // История капитала
    QList<int> _balance;

    // Определение акций доступных для покупки в данный момент (компания - цена)
    map<Company*,int> affordableStocksAndPrices(map<Company*,int> *currentRates);

    // Добавление капитала (наличные + акции) в список
    void updateBalance(int day);

    virtual void write(ofstream &out);
    virtual void read(ifstream &in);
};

#endif // HOLDER_H
