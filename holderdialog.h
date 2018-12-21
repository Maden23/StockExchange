#ifndef HOLDERDIALOG_H
#define HOLDERDIALOG_H

#include <QDialog>

namespace Ui {
class HolderDialog;
}

class HolderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HolderDialog(QWidget *parent = nullptr);
    ~HolderDialog();

    void setName(QString name);
    void setAnalysisEnabled(bool enabled);

    QString name();
    bool analysis();

private slots:
    void on_okButton_clicked();

private:
    Ui::HolderDialog *ui;
};

#endif // HOLDERDIALOG_H
