#ifndef HOLDERWITHANALYSIS_H
#define HOLDERWITHANALYSIS_H
#include "holder.h"
#include <iostream>
using namespace std;

class HolderWithAnalysis : public Holder
{
public:
    HolderWithAnalysis();
    HolderWithAnalysis(QString name, int money);
    ~HolderWithAnalysis();
    virtual info_t defineInfo() const { return IS_HOLDERWANALYSIS; }

    virtual map<Company*,int> buy(map<Company*,int> *ratesHistory, int day);
    virtual map<Company*,int> sell(map<Company*,int> *ratesHistory, int day);

protected:
    // Возвращает акции, которые может купить акционер (компания - количество)
    map<Company*,int> affordableStocks(map<Company*,int> *currentRates);

    // Возвращает минимальные возможные значения цены акции на следующий день
    map<Company*, int> predictMinPrices(map<Company *, int> *needPrediction,
                                        int day, map<Company*,int> *history);
};

#endif // HOLDERWITHANALYSIS_H
