#include "task.h"

Task::Task()
{

}

void Task::init()
{
    if (QFileInfo("config.json").exists()) {
        QFile file("config.json");
        file.open(QFile::ReadWrite);
        config = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonArray tasks = config.array();
        for (auto task : tasks) {
            auto information = task.toObject();
            QList<QString> files;
            for (auto file : information["files"].toArray()) {
                files.append(file.toString());
            }
            taskList.append(Tasks(files,
                                 information["backupFileName"].toString(),
                                 information["frequency"].toString(),
                                 information["password"].toString(),
                                 information["backupFilePath"].toString(),
                                 QDateTime::fromString(information["nextTime"].toString()),
                                 information["isCloud"].toString()));
        }
    } else {
        QFile file("config.json");
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
    }
}

void Task::addTask(Tasks task)
{
    taskList.append(task);
    writeJson();
}

void Task::deleteTask(int index)
{
    taskList.removeAt(index);
    writeJson();
}

void Task::clear()
{
    taskList.clear();
    QFile file("config.json");
    file.open(QFile::WriteOnly);
    file.write("");
    file.close();
}

void Task::updateTime(int index, QDateTime nextTime)
{
    taskList[index].nextTime = nextTime;
}

const QList<Tasks> &Task::getTaskList()
{
    return taskList;
}

void Task::writeJson()
{
    QFile file("config.json");
    file.open(QFile::WriteOnly);
    QJsonArray tasks;
    for (auto task : taskList) {
        QJsonObject information;
        QJsonArray files;
        for (auto file : task.files) {
            files.append(file);
        }
        information["files"] = files;
        information["backupFileName"] = task.bakName;
        information["frequency"] = task.freq;
        information["password"] = task.isCode;
        information["backupFilePath"] = task.localPath;
        information["nextTime"] = task.nextTime.toString();
        information["isCloud"] = task.isCloud;
        tasks.append(information);
    }
    file.write(QJsonDocument(tasks).toJson());
    file.close();
}
