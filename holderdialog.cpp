#include "holderdialog.h"
#include "ui_holderdialog.h"
#include <QMessageBox>

HolderDialog::HolderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HolderDialog)
{
    ui->setupUi(this);
}

HolderDialog::~HolderDialog()
{
    delete ui;
}

void HolderDialog::setName(QString name)
{
    ui->nameEdit->setText(name);
}

void HolderDialog::setAnalysisEnabled(bool enabled)
{
    ui->analysisBox->setEnabled(enabled);
}

QString HolderDialog::name()
{
    return ui->nameEdit->text();
}

bool HolderDialog::analysis()
{
    return ui->analysisBox->isChecked();
}

void HolderDialog::on_okButton_clicked()
{
    if (ui->nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Неполная информация", "Введите имя акционера");
        return;
    }
    else
        this->accept();
}
