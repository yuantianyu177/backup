#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QTimer>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QPixmap>
#include <QFileIconProvider>
#include <QLibrary>
#include "settingdialog.h"
#include "package.h"
#include "compress.h"
#include "decompress.h"
#include "unpackage.h"
#include "task.h"
#include "logindialog.h"
#include "md5.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void setBackupConfig(QLineEdit* backupName,
                         QLineEdit* backupDirectory,
                         QCheckBox* passwordCheckBox,
                         QLineEdit* password,
                         QRadioButton* once,
                         QRadioButton* day,
                         QRadioButton* week,
                         QCheckBox* isCloud);

    void clearBackupConfig();

    void showTasks(Tasks t);

    void setUserInfo(QLineEdit* username, QLineEdit* loginPwd);

    int getUserId();

    int registerUser(QString username, QString pwd);
    
private slots:
    void on_addFileButton_clicked();

    void on_addFoldButton_clicked();

    void on_deleteButton_clicked();

    void on_clearButton_clicked();

    void on_startButton_clicked();

    void on_settingButton_clicked();

    void on_addBackupFileButton_clicked();
    
    void on_chooseDirButton_clicked();
    
    void on_startRecoverButton_clicked();
    
    void on_pwdCheckBox_stateChanged(int arg1);

    void on_deleteTaskButton_clicked();

    void on_clearTaskButton_clicked();

    void timeout_SLOT();

    void on_refreshPushButton_clicked();

    void on_loginPushButton_clicked();

    void chooseCloudFile_SLOT();

    void on_cloudDirPushButton_clicked();

    void on_pwdCheckBox_2_stateChanged(int arg1);

    void on_cloudRecoverPushButton_clicked();

    void on_deleteCloudPushButton_clicked();

private:
    Ui::Widget *ui;
    QString backupName;
    QString backupDirectory;
    bool passwordCheckBox;
    bool once, day, week;
    bool isCloud;
    QString password;
    Task taskManager;
    QTimer timer;
    QSqlDatabase db;
    QString username;
    QString loginPwd;
    int user_id;
};

#endif // WIDGET_H
