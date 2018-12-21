#ifndef COMPANYDIALOG_H
#define COMPANYDIALOG_H

#include <QDialog>

namespace Ui {
class CompanyDialog;
}

class CompanyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompanyDialog(QWidget *parent = nullptr);
    ~CompanyDialog();

    void setName(QString name);
    void setPrice(int price);
    void setAvailable(int available);

    QString name();
    int price();
    int available();

private slots:
    void on_okButton_clicked();

private:
    Ui::CompanyDialog *ui;
};

#endif // COMPANYDIALOG_H
