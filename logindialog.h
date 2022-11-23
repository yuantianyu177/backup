#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "widget.h"

namespace Ui {
class loginDialog;
}

class Widget;

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(Widget* w, QWidget *parent = nullptr);
    ~loginDialog();

private slots:
    void on_loginPushButton_clicked();

    void on_registerPushButton_clicked();

private:
    Ui::loginDialog *ui;
    Widget* w;
    QLineEdit* username;
    QLineEdit* loginPwd;
};

#endif // LOGINDIALOG_H
