#include "companydialog.h"
#include "ui_companydialog.h"
#include <QMessageBox>


CompanyDialog::CompanyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompanyDialog)
{
    ui->setupUi(this);
}

CompanyDialog::~CompanyDialog()
{
    delete ui;
}

void CompanyDialog::setName(QString name)
{
    ui->nameEdit->setText(name);
}

void CompanyDialog::setPrice(int price)
{
    ui->priceSpinBox->setValue(price);
}

void CompanyDialog::setAvailable(int available)
{
    ui->countSpinBox->setValue(available);
}

QString CompanyDialog::name()
{
    return  ui->nameEdit->text();
}

int CompanyDialog::price()
{
    return ui->priceSpinBox->value();
}

int CompanyDialog::available()
{
    return ui->countSpinBox->value();
}


void CompanyDialog::on_okButton_clicked()
{
    if (ui->nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Неполная информация", "Введите название компании");
        return;
    }
    else
        this->accept();
}
