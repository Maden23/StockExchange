#ifndef INFO_H
#define INFO_H
#include <fstream>
#include <string>
#include <QString>

using namespace std;

enum info_t {IS_COMPANY, IS_HOLDER, IS_HOLDERWANALYSIS};

class Info
{
public:
    virtual ~Info();

    void setName(QString name);
    QString getName() const;

    virtual info_t defineInfo() const = 0;
    virtual void reset() = 0;

    // Вызывает виртуальную функцию write определенную для каждого потомка
    friend ofstream& operator<< (ofstream &out, Info &info);

    // Вызывает виртуальную функцию read определенную для каждого потомка
    friend ifstream& operator>> (ifstream &in, Info &info);

protected:
    QString _name;
    virtual void write(ofstream &out) = 0;
    virtual void read(ifstream &in) = 0;
};

#endif // INFO_H
