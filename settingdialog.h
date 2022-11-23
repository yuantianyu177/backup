#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <regex>
#include "widget.h"
namespace Ui {
class settingDialog;
}

class Widget;

class settingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingDialog(Widget* w, QWidget *parent = 0);
    ~settingDialog();

private slots:
    void on_chooseDirectoryButton_clicked();

    void on_passwordCheckBox_stateChanged(int arg1);

    void on_saveButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::settingDialog *ui;
    QLineEdit* backupName;
    QLineEdit* backupDirectory;
    QCheckBox* passwordCheckBox;
    QLineEdit* password;
    QRadioButton* once;
    QRadioButton* day;
    QRadioButton* week;
    QCheckBox* isCloud;
    Widget* w;
};

#endif // SETTINGDIALOG_H
