#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    companiesGraphWindow = nullptr;
    holdersGraphWindow = nullptr;

    filePath = "";
    fileHasChanges = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete companiesGraphWindow;
    delete holdersGraphWindow;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    ui->companiesButton->setEnabled(false);
    ui->holdersButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->newAnalysisButton->setEnabled(false);
    container = new Container<Info>();
}

void MainWindow::on_companiesButton_clicked()
{
    // Получение истории стоимости акций всех компаний
    QList<QList<int>> allHistory;
    QStringList companyNames;
    for (auto item : container->getAllItems())
    {
        if (item->defineInfo() == IS_COMPANY)
        {
            Company* company = static_cast<Company*>(item);
            allHistory.append(container->getRatesHistory(company));
            companyNames.append(company->getName());
        }
    }

    // Вызов окна, отображающего результат симуляции (график)
    if (!companiesGraphWindow)
    {
        companiesGraphWindow = new GraphWindow(this, COMPANIES_WINDOW);
    }
    companiesGraphWindow->drawCompanyGraphs(ui->daysSpinBox->value(), allHistory, companyNames);
    companiesGraphWindow->show();
}

void MainWindow::on_holdersButton_clicked()
{
    QList<Info*> allItems = container->getAllItems();

    // Проверка на наличие акционеров
    for (auto item : allItems)
    {
        if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
            break;
        // Если этот объект последний и цикл не завершился до этого момента, значит акционеров нет
        if (item == allItems.last())
        {
            QMessageBox::warning(this, "Предупреждение", "Нет акционеров для анализа");
            return;
        }
    }

    // Получение истории капитала клиентов // и истории их операций
    QList<QList<int>> holdersBalanceHistory, holdersWithAnalysisBalanceHistory;
    QStringList holdersNames, holdersWithAnalysisNames;

    for (auto item : allItems)
    {
        if (item->defineInfo() == IS_HOLDER)
        {
            Holder *holder = static_cast<Holder*>(item);
            holdersBalanceHistory.append(holder->getBalance());
            holdersNames.append(holder->getName());
        }
        else if (item->defineInfo() == IS_HOLDERWANALYSIS)
        {
            HolderWithAnalysis *holder = static_cast<HolderWithAnalysis*>(item);
            holdersWithAnalysisBalanceHistory.append(holder->getBalance());
            holdersWithAnalysisNames.append(holder->getName());
        }
    }


    // Вызов окна, отображающего результат симуляции (график и история)
    if (!holdersGraphWindow)
    {
        holdersGraphWindow = new GraphWindow(this, HOLDERS_WINDOW);
    }
    holdersGraphWindow->drawHolderGraphs(ui->daysSpinBox->value(), holdersBalanceHistory,
                    holdersWithAnalysisBalanceHistory, holdersNames, holdersWithAnalysisNames);
    holdersGraphWindow->addHoldersHistory(container->getHoldersHistoryByDay());
    holdersGraphWindow->addHoldersHistoryByHolder(container->getHoldersHistoryByHolder());
    holdersGraphWindow->show();
}

void MainWindow::on_addCompanyButton_clicked()
{

    // Создание диалога для добавления компании
    CompanyDialog *companyDialog = new CompanyDialog(this);
    // Если диалог закрыт кнопкой "Ок", добавляем новую компанию в список и в таблицу
    if (companyDialog->exec() == QDialog::Accepted)
    {
        fileHasChanges = true; // Произошло изменение файла
        addItemToTable(companyDialog->name(),IS_COMPANY);
        container->addItem(new Company(companyDialog->name(), companyDialog->price(), companyDialog->available()));
    }
    delete companyDialog;
}

void MainWindow::on_addHolderButton_clicked()
{
    // Создание диалога для добавления акционера
    HolderDialog *holderDialog = new HolderDialog(this);

    // Если диалог закрыт кнопкой "Ок", проверяем тип нового акционера
    if (holderDialog->exec() == QDialog::Accepted)
    {
        fileHasChanges = true; // Произошло изменение файла

        if (holderDialog->analysis() == true)
        {
            // Добавляем акционера с анализом в список и таблицу
            addItemToTable(holderDialog->name(), IS_HOLDERWANALYSIS);
            container->addItem(new HolderWithAnalysis(holderDialog->name(), ui->moneySpinBox->value()));
        }
        else
        {
            // Добавляем простого акционера в список и таблицу
            addItemToTable(holderDialog->name(), IS_HOLDER);
            container->addItem(new Holder(holderDialog->name(), ui->moneySpinBox->value()));
        }
    }
    delete holderDialog;
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    // При изменении данных в таблице проверяем, не пустая ли она
    if (ui->tableWidget->rowCount() > 0)
    {
        // Не пустая - активируем кнопки для анализа и удаления
        ui->beginButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
    else
    {
        // Пустая - деактивируем
        ui->beginButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }
}

void MainWindow::on_beginButton_clicked()
{
    // Проверка на существование объектов
    if (ui->tableWidget->rowCount() == 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Нет объектов для анализа");
        return;
    }

    // Проверка, что есть хотя бы один акционер и хотя бы одна компания
    bool holderFound = false;
    bool companyFound = false;
    QList <Info*> allItems = container->getAllItems();
    for (auto item : allItems)
    {
        if (item->defineInfo() == IS_COMPANY)
            companyFound = true;
        else if (item->defineInfo() == IS_HOLDER || item->defineInfo() == IS_HOLDERWANALYSIS)
            holderFound = true;

        if (companyFound && holderFound)
            break;

        if (item == allItems.last())
        {
            QMessageBox::warning(this, "Предупреждение", "Должны существовать хотя бы одна компания и хотя бы один акционер");
            return;
        }
    }

    // Данные передаются контейнеру
    container->setDays(ui->daysSpinBox->value());
    container->setHoldersMoney(ui->moneySpinBox->value());
    container->operate();

    // Отключаем кнопки изменения информации
    ui->deleteButton->setEnabled(false);
    ui->addCompanyButton->setEnabled(false);
    ui->addHolderButton->setEnabled(false);
    ui->daysSpinBox->setEnabled(false);
    ui->moneySpinBox->setEnabled(false);

    // Отключаем возможность редактирования ячеек таблицы

    // Включаем возможность посмотреть результаты и сделать новый анализ
    ui->companiesButton->setEnabled(true);
    ui->holdersButton->setEnabled(true);
    ui->newAnalysisButton->setEnabled(true);

    // Отключаем кнопку "Начать анализ"
    ui->beginButton->setEnabled(false);

    // Поиск самого успешного акционера и вывод информации о нем на экран
    Holder *winner = static_cast<Holder*>(container->getRichestHolder());

    QString winnerType;
    if (winner->defineInfo() == IS_HOLDER)
        winnerType = "простой акционер";
    else
        winnerType = "акционер с анализом";

    QMessageBox::information(this, "Информация о победившем акционере",
        "Победил акционер " + winner->getName()
        + "\nТип: " + winnerType + "\nКапитал: " + QString::number(winner->getBalance().last()));
}

void MainWindow::on_newAnalysisButton_clicked()
{
    // Возвращаем элементам контейнера изначальные значения
    container->reset();

    // Включаем кнопку "Начать анализ"
    ui->beginButton->setEnabled(true);

    // Включаем кнопки изменения информации
    ui->deleteButton->setEnabled(true);
    ui->addCompanyButton->setEnabled(true);
    ui->addHolderButton->setEnabled(true);
    ui->daysSpinBox->setEnabled(true);
    ui->moneySpinBox->setEnabled(true);

    // Отключаем возможность посмотреть результаты и сделать новый анализ
    ui->companiesButton->setEnabled(false);
    ui->holdersButton->setEnabled(false);
    ui->newAnalysisButton->setEnabled(false);
}

void MainWindow::addItemToTable(QString name, info_t type)
{
    QString typeString;
    switch (type)
    {
        case IS_COMPANY:
            typeString =  "Компания";
            break;
        case IS_HOLDER:
            typeString = "Простой акционер";
            break;
        case IS_HOLDERWANALYSIS:
            typeString = "Акционер с анализом";
    }

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(name));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(typeString));
}

void MainWindow::editCompany(Company *company, QTableWidgetItem *item, bool readOnly = false)
{
    // Флаг, сообщающий, что были изменения (есть ли необходимость вызывать reset для объекта)
    bool changesMade = false;

    // Заполнение полей формы
    CompanyDialog *dialog = new CompanyDialog(this);
    dialog->setName(company->getName());
    dialog->setPrice(company->getInitialPrice());
    dialog->setAvailable(company->getInitialAvailable());

    // Обновление данных в контейнере (если разрешено редактирование)
    if (dialog->exec() == QDialog::Accepted && readOnly == false)
    {
        // Если полученные из окна данные отличаются от имеющихся, производится обновление
        if (dialog->name() != company->getName())
        {
            // Изменение имени отражается в таблице
            ui->tableWidget->item(item->row(), 0)->setText(dialog->name());
            company->setName(dialog->name());
            changesMade = true;
        }
        if (dialog->price() != company->getInitialPrice())
        {
            company->setInitialPrice(dialog->price());
            changesMade = true;
        }
        if (dialog->available() != company->getInitialAvailable())
        {
            company->setInitialAvailable(dialog->available());
            changesMade = true;
        }
    }

    delete dialog;

    // Если произошли изменения, сбрасываем объект к начальному состоянию
    if (changesMade)
        company->reset();
}

void MainWindow::on_actionCreate_triggered()
{
    // Сохранение файла, если были изменения
    if (fileHasChanges)
    {
        // Вызов диалогового окна с вопросом о сохранении файла
        int saveReply = QMessageBox::question(this, "Сохранение файла", "Вы хотите сохранить текущий файл?",
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                              QMessageBox::Yes);

        if (saveReply == QMessageBox::Yes)
        {
            // Пользователь согласился на сохранение
            ui->actionSave->trigger(); // Вызов сохраняющего действия

            /* Цикл простаивает, пока функция сохранения не установит флаг в false,
            * то есть сохранение завершится */
            while (fileHasChanges);
        }
        else if (saveReply == QMessageBox::Cancel)
        {
            // Пользователь отменил действие
            return;
        }
    }

    // Сброс данных о текущем файле
    filePath = "";
    fileHasChanges = false;

    // Создание нового контейнера
    delete container;
    container = new Container<Info>();

    // Возвращение виджетов в изначальное состояние
    ui->tableWidget->setRowCount(0);
    ui->daysSpinBox->setValue(10);
    ui->moneySpinBox->setValue(1000);

    // Перенастройка кнопок для нового анализа
    ui->newAnalysisButton->click();

}

void MainWindow::on_daysSpinBox_valueChanged(int arg1)
{
    fileHasChanges = true; // Произошло изменение файла
}

void MainWindow::on_moneySpinBox_valueChanged(int arg1)
{
    fileHasChanges = true; // Произошло изменение файла
}

void MainWindow::on_actionOpen_triggered()
{
    // Сохранение файла, если были изменения
    if (fileHasChanges)
    {
        // Вызов диалогового окна с вопросом о сохранении файла
        int saveReply = QMessageBox::question(this, "Сохранение файла", "Вы хотите сохранить текущий файл?",
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                              QMessageBox::Yes);

        if (saveReply == QMessageBox::Yes)
        {
            // Пользователь согласился на сохранение
            ui->actionSave->trigger(); // Вызов сохраняющего действия

            /* Цикл простаивает, пока функция сохранения не установит флаг в false,
            * то есть сохранение завершится */
            while (fileHasChanges);
        }
        else if (saveReply == QMessageBox::Cancel)
        {
            // Пользователь отменил действие
            return;
        }
    }

    // Создание диалогового окна для выбора файла
    filePath = QFileDialog::getOpenFileName(this, "Открытие файла", QDir::currentPath(),
                                            "Файлы начальных данных анализа (*.stockanalysis)");

    if (filePath == "") return;

    fileHasChanges = false;

    // Создание нового контейнера
    delete container;
    container = new Container<Info>();
    int days, holderMoney;
    container->readFile(filePath, &days, &holderMoney);

    // Задание начальных значений виджетам
    ui->tableWidget->setRowCount(0);
    for (auto item : container->getAllItems())
        addItemToTable(item->getName(), item->defineInfo()); // заполнение таблицы
    ui->daysSpinBox->setValue(days);
    ui->moneySpinBox->setValue(holderMoney);

    // Перенастройка кнопок для нового анализа
    ui->newAnalysisButton->click();

}

void MainWindow::on_actionSave_triggered()
{
    // Если изменений файла не произошло, сохранять нет необходимости
    if (!fileHasChanges)
        return;

    // Если файл уже открыт, сохраняем в него
    if (filePath != "")
    {
        container->writeFile(filePath, ui->daysSpinBox->value(), ui->moneySpinBox->value());
        fileHasChanges = false;
    }
    else
    {
        // Вызывается действие "Сохранить как"
        ui->actionSaveAs->trigger();
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    // Создание диалогового окна для выбора файла для сохранения
    filePath = QFileDialog::getSaveFileName(this, "Сохранение файла", QDir::currentPath(),
                                            "Файлы начальных данных анализа (*.stockanalysis)");
    if (filePath != "")
    {
        container->writeFile(filePath, ui->daysSpinBox->value(), ui->moneySpinBox->value());
        fileHasChanges = false;
    }
}

void MainWindow::on_deleteButton_clicked()
{
    // Получение индекса выделенного элемента
    QModelIndexList indexList = ui->tableWidget->selectionModel()->selection().indexes();
    if (indexList.isEmpty())
        return; //Если ничего не выбрано, удаление не требуется
    int rowIndex = indexList.at(0).row();

    // Удаление элемента из контейнера
    QString itemName = ui->tableWidget->item(rowIndex, 0)->text();
    container->deleteItem(itemName);

    // Удаление элемента из таблицы
    ui->tableWidget->removeRow(rowIndex);


}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    // Редактирование объекта по двойному щелчку на него в таблице

    // Если кнопки для редактирования недоступны (например, "добавить компанию"),
    // значит редактирование в данный момент запрещено
    bool readOnly = false;
    if (!ui->addCompanyButton->isEnabled())
        readOnly = true;;

    // Получение имени объекта
    QString objectName = ui->tableWidget->item(item->row(), 0)->text(); // Столбец 0 - имя

    // Получение указателя на соответствующий объект в контейнере
    Info* object = container->getItem(objectName);

    // Создание форм соответствующего типа и обновление информации
    if (object->defineInfo() == IS_COMPANY)
    {
        Company *company = static_cast<Company*>(object);
        editCompany(company, item, readOnly);
    }
    else
    {
        // У акционера можно поменять только имя, тип менять запрещается
        HolderDialog *dialog = new HolderDialog(this);
        dialog->setName(objectName);
        dialog->setAnalysisEnabled(false);

        // Меняем значение, если оно отличается и доступно редактирование
        if (dialog->exec() == QDialog::Accepted && !readOnly
                && dialog->name() != objectName)
        {
            // Обновление в контейнере и в таблице
            object->setName(dialog->name());
            ui->tableWidget->item(item->row(),0)->setText(dialog->name());
        }
        delete dialog;
    }

}
