#include "container.h"
#include <fstream>
#include <iostream>
using namespace std;

template<class T>
Container<T>::Container()
{
    _days = 10;
    _ratesHistory = new map<Company*,int>[_days];
}

template<class T>
Container<T>::Container(int days)
{
    (days > 0) ? _days = days : _days = 10;
    _ratesHistory = new map<Company*,int>[_days];
}

template<class T>
Container<T>::~Container()
{

    delete [] _ratesHistory;
}

template<class T>
void Container<T>::setDays(int days)
{
    if (days > 0)
        _days = days;

    if (_ratesHistory)
        delete _ratesHistory;

    _ratesHistory = new map<Company*,int>[_days];
}

template<class T>
void Container<T>::setAllItems(const QList<T *> &items)
{
    _items = items;
}

template<class T>
QList<int> Container<T>::getRatesHistory(Company *company)
{
    QList<int> history;
    for (int day = 0; day < _days; day++)
    {
        history.append(_ratesHistory[day][company]);
    }
    return history;
}

template<class T>
QList<int> Container<T>::getRatesHistory(QString companyName)
{
    Company *company = static_cast<Company*>(getItem(companyName));
    QList<int> history;
    for (int day = 0; day < _days; day++)
    {
        history.append(_ratesHistory[day][company]);
    }
    return history;
}

template<class T>
T *Container<T>::getItem(QString name)
{
    for (auto item : _items)
    {
        if (item->getName() == name)
            return item;
    }
    return nullptr;
}

template<class T>
void Container<T>::operate()
{
    for (int day = 0; day < _days; day++)
    {
        //Обновление цен акций
        for (auto item : _items)
        {
            if (item->defineInfo() == IS_COMPANY)
            {
                Company *companyItem = static_cast<Company*>(item);
                // В 0-й день записывается изначальная цена, далее пересчитывается
                if (day == 0)
                {
                    _ratesHistory[day][companyItem] = companyItem->getInitialPrice();
                }
                else
                {
                    int newPrice = companyItem->changePrice();
                    _ratesHistory[day][companyItem] = newPrice;
                }
            }
        }

        //Покупка-продажа акций
        for (auto item : _items)
        {
            if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
            {
                Holder *holderItem = static_cast<Holder*>(item);
                holderItem->sell(_ratesHistory, day);
                holderItem->buy(_ratesHistory, day);
            }
        }

    }
}

template<class T>
void Container<T>::reset()
{
    for (auto item : _items)
    {
        item->reset();
    }
}

template<class T>
T *Container<T>::getRichestHolder()
{
    int maxBalance = 0;
    Holder *winner;
    Holder *holder;
    // Поиск максимального значения среди балансов акционеров за последний день
    for (auto item : _items)
    {
        if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
        {
           holder = static_cast<Holder*>(item);
           int balance = holder->getBalance().last();
           if (balance > maxBalance)
           {
               maxBalance = balance;
               winner = holder;
           }
        }
    }
    return winner;
}

template<class T>
QList<QStringList> Container<T>::getHoldersHistoryByDay()
{
    // Общая история операций по дням
    QList<QStringList> history;
    for (int i = 0; i < _days; i++)
        history.append(QStringList());

    for (auto item : _items)
    {
        if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
        {
            // Получение истории операций акционера
            Holder *holder = static_cast<Holder*>(item);
            QList<Record> holderHistory = holder->getHistory();

            // Каждый список операций акционера добавляется в общую историю за соответствующий день
            for (auto record : holderHistory)
            {
                // Получение типа операций в списке
                QString operationType;
                if (record.recordType == BOUGHT)
                    operationType = " купил ";
                else
                    operationType = " продал";

                // Формирование из списка операций строки с операцией по каждому пакету акций
                QString newHistoryRecord;
                for (auto stock : record.stocks)
                {
                    QString stockCount = QString::number(stock.second);
                    QString companyName = stock.first->getName();
                    newHistoryRecord = "Акционер " + holder->getName()
                                     + operationType
                                     + stockCount + " акций компании " + companyName;

                    // Добавление строки в общую историю за нужный день
                    history[record.day].append(newHistoryRecord);
                }

            }
        }
    }

    return history;
}

template<class T>
map<QString, QStringList> Container<T>::getHoldersHistoryByHolder()
{
    map<QString, QStringList> history;

    for (auto item : _items)
    {
        if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
        {
            // Получение истории операций акционера
            Holder *holder = static_cast<Holder*>(item);
            QList<Record> holderHistory = holder->getHistory();

            // Каждый список операций акционера добавляется в общую историю
            for (auto record : holderHistory)
            {
                // Получение типа операций в списке
                QString operationType;
                if (record.recordType == BOUGHT)
                    operationType = " купил ";
                else
                    operationType = " продал ";

                // Формирование из списка операций строки с операцией по каждому пакету акций
                QString newHistoryRecord;
                for (auto stock : record.stocks)
                {
                    QString stockCount = QString::number(stock.second);
                    QString companyName = stock.first->getName();
                    newHistoryRecord = "День " + QString::number(record.day + 1) + ": "
                                     + "Акционер " + holder->getName()
                                     + operationType
                                     + stockCount + " акций компании " + companyName;

                    // Добавление строки в общую историю по имени акционера
                    history[item->getName()].append(newHistoryRecord);
                }

            }
        }
    }

    return history;
}

template<class T>
//запись данных в текстовый файл
void Container<T>::writeFile(const QString fileName, int days, int holdersMoney)
{
    // Создание потока для записи в файл
    std::ofstream fout(fileName.toStdString());

    fout << days << endl
         << holdersMoney << endl;

    for (auto item : _items)
    {
        fout << item->defineInfo() << "|";
        fout << *item;
    }


    fout.close();
}

template<class T>
//загрузка данных из текстового файла
void Container<T>::readFile(const QString fileName, int *days, int *holdersMoney)
{
    ifstream fin(fileName.toStdString());

    fin >> *days >> *holdersMoney;

    int currType;
    Info * currItem = nullptr;

    while (fin >> currType) // Считывание типа объекта - false, если считывание не удалось
    {
        // Создание объекта нужного типа
        switch(currType)
        {
            case IS_COMPANY:
                currItem = new Company();
                break;
            case IS_HOLDER:
                currItem = new Holder();
                break;
            case IS_HOLDERWANALYSIS:
                currItem = new HolderWithAnalysis();
                break;
            default:
                //qDebug() << "Неизвестный тип объекта в файле";
                return;
        }
        if (currItem)
        {
            // Чтение значений для объекта
            fin >> *currItem;

            // Добавление объекта в контейнер
            addItem(currItem);
        }
    }

    fin.close();
}

template<class T>
void Container<T>::addItem(T *newItem)
{
    // Проверка на существование аналогичного элемента
    if (newItem && !getItem(newItem->getName()))
        _items.append(newItem);
}

template<class T>
void Container<T>::deleteItem(QString name)
{
    T* item = getItem(name);
    if (item)
    {
        _items.removeOne(item);
        delete item;
    }
}

template<class T>
void Container<T>::setHoldersMoney(int money)
{
    for (auto item : _items)
    {
        if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
        {
            Holder *holder = static_cast<Holder*>(item);
            holder->setMoney(money);
        }
    }
}

// Сообщение компилятору, что необходимо скомпилировать этот код для типа Info
// https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function
template class Container<Info>;
