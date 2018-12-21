#include "holder.h"
#include "company.h"
#include <QDebug>

Holder::Holder()
{
    _name = "DefaultString";
    _money = 10000;
}

Holder::Holder(QString name, int money)
{
    (name != "") ? _name = name : _name = "DefaultString";
    (money != 0) ? _money = money : _money = 10000;
}

Holder::~Holder()
{

}

map <Company*, int> Holder::buy(map<Company *, int> *ratesHistory, int day)
{
    map <Company*,int> bought; // Словарь для записи купленных акций
    // Получение словаря доступных акций (акция - цена)
    map<Company*,int> affordable = affordableStocksAndPrices(&ratesHistory[day]);

    // Определение количества покупаемых акций
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_int_distribution<> affordableDistr(0 , affordable.size());
    int numberOfCompanies = affordableDistr(generator); // Количество акций для покупки (будет уменьшаться с каждой итерацией)

    // Пока есть возможность купить акции и не достигнуто нужное количество, покупаются акции случайной компании
    while (!affordable.empty() && numberOfCompanies-- > 0)
    {
        int companyId = affordableDistr(generator); // номер нужной случайной компании в словаре
        int id = 0;
        for (auto stock : affordable)
        {
            if (id++ == companyId) // Найдена компания, акции которой будут куплены
            {
                // Определение количества акций
                static uniform_int_distribution<> stocksDistr(1, stock.first->getAvailable());
                int stocksCount = stocksDistr(generator);
                 // Пока есть деньги для покупки и не достигнуто нужное количество, покупается акция
                while (_money > stock.second && stocksCount-- > 0)
                {
                    // Покупка
                    _stocks[stock.first] += 1; // Увеличение количества имеющихся акций на 1
                    _money -= stock.second; // Уменьшение денег на цену акции
                    stock.first->changeAvailable(-1); // Уменьшение количества доступнах акций компании
                    bought[stock.first] += 1; // Добавление записи о продаже в возвращаемый словарь
                }
            }
        }

        affordable = affordableStocksAndPrices(&ratesHistory[day]);
    }
    // Запись в историю покупок-продаж, если что-то было куплено
    if (!bought.empty())
    {
        Record newRecord;
        newRecord.day = day;
        newRecord.recordType = BOUGHT;
        newRecord.stocks = bought;
        _history.append(newRecord);

    }
    // Обновление баланса
    updateBalance(day);

    return bought;
}

map <Company*, int> Holder::sell(map<Company*,int> *ratesHistory, int day)
{
    map <Company*,int> sold; // Словарь для записи проданных акций
    if (!_stocks.empty())
    {
        // Определение случайного числа компаний из имеющихся, акции которых будут проданы
        static random_device rd;
        static mt19937 generator(rd());
        static uniform_int_distribution<> companiesDistr(0, _stocks.size()); // Распределение от 0 до количества компаний
        int numberOfCompanies = companiesDistr(generator);

        // Распределение для получения порядкового номера компании в map
        static uniform_int_distribution<> idDistr(1, numberOfCompanies);
        // Поиск очередного элемента numberOfCompanies раз
        for (int i = 0; i < numberOfCompanies; i++)
        {
            // Получение случайного номера
            int id = idDistr(generator);
            int currId = 0;
            for (auto stock : _stocks)
            {
                if (currId++ == id) // Найден элемент для продажи
                {
                    // Получение количества акций для продажи
                    static uniform_int_distribution<> stocksDistr(1, stock.second);
                    int numberOfStocks = stocksDistr(generator);
                    // Продажа
                    stock.second -=numberOfStocks; // уменьшение количества имеющихся акций
                    _money += stock.first->getPrice() * numberOfStocks; // увеличение денег
                    stock.first->changeAvailable(numberOfStocks); // увеличение количества доступнах акций компании на число проданных
                    sold[stock.first] += numberOfStocks; // добавление записи о продаже в возвращаемый словарь
                }
            }
            // Если количество акций компании стало равным нулю, удаляем компанию из имеющихся
            for (auto i = _stocks.cbegin(); i != _stocks.cend(); )
            {
                if (i->second == 0)
                {
                    _stocks.erase(i);
                }
                else
                {
                    i++;
                }
            }
        }
    }
    // Запись в историю покупок-продаж, если что-то было продано
    if (!sold.empty())
    {
        Record newRecord;
        newRecord.day = day;
        newRecord.recordType = SOLD;
        newRecord.stocks = sold;
        _history.append(newRecord);
    }

    // Обновление баланса
    updateBalance(day);


    return sold;
}

void Holder::reset()
{
    _balance.clear();
    _history.clear();
    _stocks.clear();
}

void Holder::setMoney(int money)
{
    if (money > 0)
        _money = money;
}

void Holder::write(ofstream &out)
{
    out << _name.toStdString() << endl;
}

void Holder::read(ifstream &in)
{
    in.ignore(); // Удаление символов в буфере потока (переводы строки)
    string name = "";
    getline(in, name);
    setName(QString::fromStdString(name));
}

map<Company *, int> Holder::affordableStocksAndPrices(map<Company *, int> *currentRates)
{
    map<Company*,int> affordable;
    // Если имеется больше денег, чем стоит акция,
    // и есть доступные акции, она добавляется к возможным для покупки
    for (auto stock : *currentRates)
    {
        if (_money > stock.second && stock.first->getAvailable() > 0)
        {
            affordable.insert(stock);
        }
    }
    return affordable;
}

void Holder::updateBalance(int day)
{
    int balance = _money;
    for (auto stock : _stocks)
    {
        // К капиталу добавляется стоимость акции, умноженная на их количество
        balance += stock.first->getPrice() * stock.second;
    }

    // Добавление полученного значения к истории капитала
    if (_balance.size() == day + 1)  // Значение на данный день уже существует
        _balance[day] = balance; // Обновление существующего значения
    else
        _balance.append(balance); // Добавление нового значения
}


