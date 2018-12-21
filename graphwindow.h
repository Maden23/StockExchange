#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QDialog>
#include <map>

using namespace std;

enum graphwindow_t {HOLDERS_WINDOW, COMPANIES_WINDOW};

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GraphWindow( QWidget *parent = nullptr, graphwindow_t type = HOLDERS_WINDOW);
    ~GraphWindow();

    void drawCompanyGraphs(int days, const QList<QList<int>> &data, const QStringList &names);

    void drawHolderGraphs(int days, const QList<QList<int>> &holders, const QList<QList<int>> &holdersWithAnalysis,
                          const QStringList &holdersNames, const QStringList &holdersWithAnalysisNames);

    void addHoldersHistory(const QList<QStringList> &historyByDays);
    void addHoldersHistoryByHolder(const map<QString, QStringList> &historyByHolder);

protected:
      void showEvent(QShowEvent *event);

private slots:
      void on_holdersComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::GraphWindow *ui;

    // История операций акционеров
    QList<QStringList> _historyByDays;
    map<QString, QStringList> _historyByHolder;
    void showAllHistory();
    void showHolderHistory(QString holder);

    // Рисование одной линии графа
    void drawGraphLine(int days, QList<int> values, QPen pen, QString name);

    // Нахождение максимального и минимального значения по y
    double findMaxYValue(const QList<QList<int>> &values);
    double findMinYValue(const QList<QList<int>> &values);
};

#endif // GraphWindow_H
