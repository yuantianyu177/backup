#include "settingdialog.h"
#include "widget.h"
#include "ui_settingdialog.h"

settingDialog::settingDialog(Widget *w, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingDialog)
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    this->w = w;
    this->backupName = nullptr;
    this->backupDirectory = nullptr;
    this->passwordCheckBox = nullptr;
    this->password = nullptr;
    this->once = nullptr;
    this->day = nullptr;
    this->week = nullptr;
    this->isCloud = nullptr;
}

settingDialog::~settingDialog()
{
    delete ui;
}

void settingDialog::on_chooseDirectoryButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("备份到"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directory != "") {
        ui->backupDirectoryLineEdit->setText(directory);
    }
}

void settingDialog::on_passwordCheckBox_stateChanged(int arg1)
{
    ui->passwordLineEdit->setEnabled(ui->passwordCheckBox->checkState());
}

void settingDialog::on_saveButton_clicked()
{
    backupName = ui->backupNameLineEdit;
    backupDirectory = ui->backupDirectoryLineEdit;
    passwordCheckBox = ui->passwordCheckBox;
    password = ui->passwordLineEdit;
    once = ui->onceRadioButton;
    day = ui->dayRadioButton;
    week = ui->weekRadioButton;
    isCloud = ui->cloudCheckBox;

    if(backupName->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入备份名。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(backupDirectory->text().trimmed() == "")
    {
        QMessageBox::information(this, "提示", "请输入备份目录。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    std::regex filenameExpress("[\\/:*?\"<>|]");
    if (std::regex_search(backupName->text().toStdString(), filenameExpress)) {
        QMessageBox::information(this, "提示", "请输入合法的备份文件名。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if (QFileInfo(backupDirectory->text() + "\\" + backupName->text()).exists()) {
        if (QMessageBox::Yes != QMessageBox::question(this, "警告", "文件已存在，确认覆盖？", QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }

    if (passwordCheckBox->isChecked() && password->text().trimmed() == "") {
        QMessageBox::information(this, "提示", "请输入密码。", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    if(isCloud->isChecked() && w->getUserId() == -1) {
        QMessageBox::information(this, "提示", "请先登录", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    w->setBackupConfig(this->backupName,
                       this->backupDirectory,
                       this->passwordCheckBox,
                       this->password,
                       this->once,
                       this->day,
                       this->week,
                       this->isCloud);
    this->close();
}

void settingDialog::on_cancelButton_clicked()
{
    this->close();
    this->~settingDialog();
}
