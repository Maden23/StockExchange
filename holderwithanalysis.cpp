#include "holderwithanalysis.h"

HolderWithAnalysis::HolderWithAnalysis()
{
    _name = "DefaultString";
    _money = 10000;
}

HolderWithAnalysis::HolderWithAnalysis(QString name, int money)
{
    (name != "") ? _name = name : _name = "DefaultString";
    (money != 0) ? _money = money : _money = 10000;
}

HolderWithAnalysis::~HolderWithAnalysis()
{

}

map<Company *, int> HolderWithAnalysis::buy(map<Company *, int> *ratesHistory, int day)
{
    /* Покупаются акции одной компании, выбираемой по алгоритму:
     * 1. Находятся компании, акции которых акционер может купить (компания и количество акций по карману).
     * 2. Для каждой найденной компании считается минимальная возможная цена акции на следующий день.
     * 3. Считается минимальная возможная стоимость пакета акций на следующий день (мин. цена * количество акций по карману).
     * 4. Пакет акций с наибольшей мин. стоимостью покупается.
     */

    map <Company*,int> bought; // Словарь для записи купленных акций

    // Получение словаря доступных акций (компания - количество)
    map<Company*,int> affordable = affordableStocks(&ratesHistory[day]);

    // Если есть доступные акции, производится анализ
    if (!affordable.empty())
    {
        // Подсчет минимальных цен на одну акцию
        map<Company*, int> minimalPrices = predictMinPrices(&affordable, day, ratesHistory);

        // Подсчет стоимости пакетов
        map <Company*, int> multipleStocksPrices;
        for (auto affordableStock : affordable)
        {
            Company* company = affordableStock.first;
            // Стоимость пакета = цена акции * количество доступных для покупки
            multipleStocksPrices[company] =  minimalPrices[company] * affordableStock.second;
        }

        // Поиск пакета с наибольшей минимальной стоимостью
        int stocksMaxMinValue = 0; // Максимальная стоимость пакета
        Company* company; // Компания с самым дорогим пакетом
        for (auto mult : multipleStocksPrices)
        {
            if (mult.second > stocksMaxMinValue)
            {
                stocksMaxMinValue = mult.second;
                company = mult.first;
            }
        }

        // Покупка
        int stocksCount = affordable[company]; // Количество покупаемых акций
        _stocks[company] += stocksCount; // Увеличение количества имеющихся акций на количество доступных к покупке
        _money -= stocksMaxMinValue; // Уменьшение денег на стоимость пакета
        company->changeAvailable(-stocksCount); // Уменьшение количества доступнах акций компании
        bought[company] += stocksCount; // Добавление записи о продаже в возвращаемый словарь

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

map<Company *, int> HolderWithAnalysis::sell(map<Company *, int> *ratesHistory, int day)
{
    // Продаются те акции, которые упали в прошлый раз и могут принести наибольшие убытки

    map<Company*, int> sold;

    // Если есть акции для продажи - производится анализ
    if (!_stocks.empty())
    {
        // Формирование списка упавших акций из имеющихся
        map<Company*, int> falling;
        for (auto stock : _stocks)
        {
            // Проверка на падение цены
            int currPrice = ratesHistory[day][stock.first]; // Текущая цена акции
            int prevPrice = ratesHistory[day-1][stock.first];
            if (currPrice < prevPrice)
            {
                falling.insert(stock);
            }

        }
        // Анализ продолжается если есть падающие акции
        if (!falling.empty())
        {
            // Получение предполагаемых цен на акции
            map <Company*, int> minPrices = predictMinPrices(&falling, day, ratesHistory);

            // Нахождение пакета, который может принести наибольший убыток
            int maxLoss = 0;
            Company *badCompany = nullptr;
            for (auto stock : falling)
            {
                int currPrice = ratesHistory[day][stock.first] * stock.second; // Текущая стоимость пакета
                int badPrice = minPrices[stock.first] * stock.second;
                if (currPrice - badPrice > maxLoss)
                {
                    maxLoss = currPrice - badPrice;
                    badCompany = stock.first;
                }
            }

            // Продажа
            int numberOfStocks = falling[badCompany]; // количество продаваемых акций
            _stocks.erase(badCompany); // удаление имеющихся акций

            _money += badCompany->getPrice() * numberOfStocks; // увеличение денег на стоимость пакета
            badCompany->changeAvailable(numberOfStocks); // увеличение количества доступнах акций компании на число проданных
            sold[badCompany] += numberOfStocks; // добавление записи о продаже в возвращаемый словарь

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

map<Company *, int> HolderWithAnalysis::affordableStocks(map<Company *, int> *currentRates)
{
    // Возвращает акции, которые может купить акционер (компания - количество)
    map<Company*, int> affordable;
    // проверять наличие акций

    for (auto rate : *currentRates)
    {
        int maxCount = rate.first->getAvailable(); //Акции в продаже
        int count = 0; // Счетчик акций, которые акционер может купить

        // Значение не может быть больше количества акций в продаже,
        // стоимость акций не должна превышать деньги
        while ((count <= maxCount) && (count * rate.second <= _money))
        {
            count++;
        }

        // Отнимаем последнюю посчитанную акцию, потому что на ней не выполнились условия
        count--;

        // Если нашлись доступные акции - добавляем к словарю
        if (count > 0)
            affordable[rate.first] = count;


    }

    return affordable;
}

map<Company *, int> HolderWithAnalysis::predictMinPrices(map<Company*,int> *needPrediction,
                                                int day, map<Company*,int> *history)
{
    // Если акция выросла на предыдущей итерации, то ее падение может быть до -10%
    // Если упала, то до -20%

    map<Company*, int> minPrices;

    for (auto company : *needPrediction)
    {
        int currPrice = history[day][company.first]; // Текущая цена акции

        // Проверка на падение цены
        bool falling; // Флаг падения в предыдущий день (в 0-й день false)
        if (day == 0)
            falling = false;
        else
        {
            // Сравнение цены акции с предыдущей
            int prevPrice = history[day-1][company.first];
            if (currPrice >= prevPrice)
                falling = false;
            else
                falling = true;
        }

        // Предсказание цены
        int newPrice;
        if (falling)
            newPrice = currPrice - currPrice*20/100;
        else
            newPrice = currPrice - currPrice*10/100;

        // Занесение предсказанной цены в результат
        minPrices[company.first] = newPrice;
    }

    return minPrices;
}

