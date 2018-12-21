#include "graphwindow.h"
#include "ui_graphwindow.h"

GraphWindow::GraphWindow(QWidget *parent, graphwindow_t type) :
    QDialog(parent),
    ui(new Ui::GraphWindow)
{
    ui->setupUi(this);
    if (type == COMPANIES_WINDOW)
    {
        ui->holdersHistoryWidget->hide();
    }

}

GraphWindow::~GraphWindow()
{
    delete ui;
}

void GraphWindow::drawCompanyGraphs(int days, const QList<QList<int> > &data, const QStringList &names)
{
    // Сброс области графика
    ui->graphPlot->clearPlottables();

    // Подготовка оси x
    ui->graphPlot->xAxis->setLabel("Дни");
    ui->graphPlot->xAxis->setRange(1, days);

    // Подготовка оси y
    ui->graphPlot->yAxis->setLabel("Ценность");
    ui->graphPlot->yAxis->setRange(0, findMaxYValue(data));

    // Подготовка легенды
    ui->graphPlot->legend->setVisible(true);
    ui->graphPlot->legend->setFont(QFont("Helvetica", 9));

    // Цвет и стиль линий
    QList<QColor> colors = { QColor("magenta"), QColor("red"), QColor("darkCyan"), QColor("yellow"),
                             QColor("darkMagenta"), QColor("green"), QColor("blue"),  QColor("cyan"),
                             QColor("darkRed"), QColor("darkGreen")};


    QPen pen;
    pen.setWidth(2);
    int currColNum = 0; // индекс текущего цвета

    // Рисование отдельных графиков
    for (int companyIndex = 0; companyIndex < data.size(); companyIndex++)
    {
        QColor currentColor(colors[currColNum]);
        pen.setColor(currentColor);
        drawGraphLine(days, data[companyIndex], pen, names[companyIndex]);

        // Для каждой новой линии используется новый цвет
        currColNum++;
        if (currColNum == colors.size())
        {
            currColNum = 0;
        }
    }

}

void GraphWindow::addHoldersHistory(const QList<QStringList> &historyByDays)
{
    // Сохранение истории
    _historyByDays = historyByDays;

    // Отображение истории операций
    showAllHistory();
}

void GraphWindow::addHoldersHistoryByHolder(const map<QString, QStringList> &historyByHolder)
{
    // Сохранение истории
    _historyByHolder = historyByHolder;

    // Формирование списка имен акционеров
    QStringList holderNames;
    for (auto holder : historyByHolder)
        holderNames << holder.first;

    // Добавление значений в выпадающий список
    ui->holdersComboBox->clear();
    ui->holdersComboBox->addItem("");
    ui->holdersComboBox->addItems(holderNames);

}

void GraphWindow::drawHolderGraphs(int days, const QList<QList<int> > &holders, const QList<QList<int>> &holdersWithAnalysis,
                                   const QStringList &holdersNames, const QStringList &holdersWithAnalysisNames)
{
    // Сброс области графика
    ui->graphPlot->clearPlottables();

    // Подготовка оси x
    ui->graphPlot->xAxis->setLabel("Дни");
    ui->graphPlot->xAxis->setRange(1, days);

    // Подготовка оси y
    ui->graphPlot->yAxis->setLabel("Капитал");
    QList<QList<int>> allValues;
    allValues << holders << holdersWithAnalysis;
    double minValue = findMinYValue(allValues);
    double maxValue =  findMaxYValue(allValues);
    double margin = (maxValue - minValue)*0.1;
    ui->graphPlot->yAxis->setRange(minValue - margin, maxValue + margin);

    // Подготовка легенды
    ui->graphPlot->legend->setVisible(true);
    ui->graphPlot->legend->setFont(QFont("Helvetica", 9));

    // Цвет и стиль линий
    QList<QColor> colors = { QColor("magenta"), QColor("red"), QColor("darkCyan"), QColor("yellow"),
                             QColor("darkMagenta"), QColor("green"), QColor("blue"),  QColor("cyan"),
                             QColor("darkRed"), QColor("darkGreen")};

    QPen pen;
    Qt::PenStyle penStyle = Qt::SolidLine;
    pen.setStyle(penStyle);
    pen.setWidth(2);

    int currColNum = 0; // индекс текущего цвета

    // Рисование графиков для простых акционеров
    for (int holderIndex = 0; holderIndex < holders.size(); holderIndex++)
    {
        QColor currentColor(colors[currColNum]);
        pen.setColor(currentColor);
        drawGraphLine(days, holders[holderIndex], pen, holdersNames[holderIndex]);

        // Для каждой новой линии используется новый цвет
        // Всего 10 цветов => возможны 10 акционеров
        currColNum++;
        if (currColNum == colors.size())
        {
            currColNum = 0;
        }
    }

    // Рисование графиков для акционеров с анализом (линия меняется на пунктир)
    penStyle = Qt::DashDotLine;
    pen.setStyle(penStyle);
    pen.setWidth(3);
    currColNum = 0;

    for (int holderIndex = 0; holderIndex < holdersWithAnalysis.size(); holderIndex++)
    {
        QColor currentColor(colors[currColNum]);
        pen.setColor(currentColor);
        drawGraphLine(days, holdersWithAnalysis[holderIndex], pen, holdersWithAnalysisNames[holderIndex]);

        // Для каждой новой линии используется новый цвет
        currColNum++;
        if (currColNum == colors.size())
        {
            currColNum = 0;
        }
    }
}

void GraphWindow::drawGraphLine(int days, QList<int> values, QPen pen, QString name)
{
    // Преобразование данных в QVector<double>
    QVector<double> daysData;
    for (int day = 1; day <= days; day++)
        daysData.append(day);
    QVector<double> valuesData;
    for (auto value : values)
        valuesData.append(value);

    ui->graphPlot->addGraph();
    ui->graphPlot->graph()->setPen(pen);
    ui->graphPlot->graph()->setName(name);
    ui->graphPlot->graph()->addData(daysData, valuesData);
    ui->graphPlot->replot();
}

double GraphWindow::findMaxYValue(const QList<QList<int> > &values)
{
    double maxValue = 0;
    for (auto valueList : values)
    {
        for (auto value : valueList)
        {
            if (value > maxValue)
                maxValue = value;
        }
    }
    return maxValue;
}

double GraphWindow::findMinYValue(const QList<QList<int> > &values)
{
    double minValue = INT_MAX;
    for (auto valueList : values)
    {
        for (auto value : valueList)
        {
            if (value < minValue)
                minValue = value;
        }
    }
    return minValue;
}

void GraphWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

}

void GraphWindow::showAllHistory()
{
    ui->plainTextEdit->clear();
    for (int i = 0; i < _historyByDays.size(); i++)
        for (auto line : _historyByDays[i])
            ui->plainTextEdit->appendPlainText("День " + QString::number(i+1) + ": " + line);
}

void GraphWindow::showHolderHistory(QString holder)
{
    ui->plainTextEdit->clear();

    for (auto line : _historyByHolder[holder])
        ui->plainTextEdit->appendPlainText(line);
}


void GraphWindow::on_holdersComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "")
        showAllHistory();
    else
        showHolderHistory(arg1);
}
