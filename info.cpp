#include "info.h"

Info::~Info()
{

}

void Info::setName(QString name)
{
    //Модификатор поля name
    //Принимает значение поля name
    //Ничего не возвращает

    if (name != "")
        _name = name;
}

QString Info::getName() const
{
    //Селектор поля name
    //Ничего не принимает
    //Возвращает значение поля name

    return _name;
}

ifstream &operator>>(ifstream &in, Info &info)
{
    info.read(in);
    return in;
}

ofstream &operator<<(ofstream &out, Info &info)
{
    info.write(out);
    return out;
}

