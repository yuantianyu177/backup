#include "logindialog.h"
#include "ui_logindialog.h"
#include "widget.h"

loginDialog::loginDialog(Widget *w, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    ui->pwdLineEdit->setEchoMode(QLineEdit::Password);
    this->username = nullptr;
    this->loginPwd = nullptr;
    this->w = w;
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::on_loginPushButton_clicked()
{
    username = ui->userNameLineEdit;
    loginPwd = ui->pwdLineEdit;

    if(username->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入用户名。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(loginPwd->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入密码。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    w->setUserInfo(username, loginPwd);
    this->close();
}

void loginDialog::on_registerPushButton_clicked()
{
    username = ui->userNameLineEdit;
    loginPwd = ui->pwdLineEdit;

    if(username->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入用户名。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(loginPwd->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入密码。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(w->registerUser(username->text(), loginPwd->text()))
    {
        QMessageBox::information(this, "提示", "用户名已存在", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QMessageBox::information(this, "提示", "注册成功", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
}

