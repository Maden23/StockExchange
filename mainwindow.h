#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include "graphwindow.h"
#include "companydialog.h"
#include "holderdialog.h"
#include "container.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
      void showEvent(QShowEvent *event);

private slots:
      void on_companiesButton_clicked();

      void on_holdersButton_clicked();

      void on_addCompanyButton_clicked();

      void on_addHolderButton_clicked();

      void on_tableWidget_itemChanged(QTableWidgetItem *item);

      void on_beginButton_clicked();

      void on_newAnalysisButton_clicked();

      void on_actionCreate_triggered();

      void on_daysSpinBox_valueChanged(int arg1);

      void on_moneySpinBox_valueChanged(int arg1);

      void on_actionOpen_triggered();

      void on_actionSave_triggered();

      void on_actionSaveAs_triggered();

      void on_deleteButton_clicked();

      void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    GraphWindow *companiesGraphWindow;
    GraphWindow *holdersGraphWindow;
    Container<Info> *container;
    int days;

    bool fileHasChanges;
    QString filePath;

    void addItemToTable(QString name, info_t type);
    void editCompany(Company *company, QTableWidgetItem *item, bool readOnly);
};

#endif // MAINWINDOW_H
