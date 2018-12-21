#ifndef CONTAINER_H
#define CONTAINER_H
#include "holder.h"
#include "holderwithanalysis.h"
#include "company.h"
#include <QList>

using namespace std;

template<class T>
class Container
{
public:

    Container();
    Container(int days);
    Container(const Container &other);

    ~Container();

    // Действующие компании и акционеры
    QList<T*> getAllItems() const {return _items;}
    void setAllItems(const QList<T*> &items);

    // История котировок акций по дням
    QList<int> getRatesHistory(Company* company);
    QList<int> getRatesHistory(QString companyName);

    // Действия с конкретным объектом
    T* getItem(QString name);
    void addItem(T *newItem);
    void deleteItem(QString name);

    // Действия с контейнером
    void setHoldersMoney(int money);
    void setDays(int days); // Назначить количество дней симуляции
    void operate(); // Провести симуляцию
    void reset(); // Сбросить параметры всех объектов до начального состояния
    T* getRichestHolder(); // Возвращает указатель на самого успешного акционера
    QList<QStringList> getHoldersHistoryByDay(); // Возвращает историю операций акционеров по дням
    map<QString, QStringList> getHoldersHistoryByHolder(); // Возвращает историю операций акционеров по каждому

    // Действия с файлами
    void writeFile(const QString fileName, int days, int holdersMoney); //запись данных в текстовый файл
    void readFile(const QString fileName, int *days, int *holdersMoney); //чтение данных из текстового файла

private:
    int _days;
    // Действующие компании и акционеры
    QList<T*> _items;
    // История котировок акций по дням
    map<Company*,int> *_ratesHistory;
};



#endif // CONTAINER_H
