#ifndef PACKAGE_H
#define PACKAGE_H
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
class Package
{
public:

    static void shortCutTackle(QFile* package, QFileInfo info, QString root)
    {
        int flag = 1; //快捷方式标志位
        package->write((const char*)&flag, sizeof(flag));
        QString symLinkTarget = info.symLinkTarget();
        int length = strlen(symLinkTarget.toStdString().c_str());
        package->write((const char*)&length, sizeof(length));
        package->write(symLinkTarget.toStdString().c_str());
        //获取相对路径
        QString relativePath = QString(info.absoluteFilePath()).replace(root, "");
        int pathLength = strlen(relativePath.toStdString().c_str());
        //写相对路径长度
        package->write((const char*)&pathLength, sizeof(pathLength));
        //写相对路径名
        package->write(relativePath.toStdString().c_str());
    }

    static void commomFileTackle(QFile* package, QFileInfo info, QString root)
    {
        int flag = 0; //快捷方式标志位
        package->write((const char*)&flag, sizeof(flag));
        //获取相对路径
        QString relativePath = info.absoluteFilePath().replace(root, "");
        int pathLength = strlen(relativePath.toStdString().c_str());
        //写相对路径长度
        package->write((const char*)&pathLength, sizeof(pathLength));
        //写相对路径名
        package->write(relativePath.toStdString().c_str());
        //写文件时间
        qint64 birthTime = info.birthTime().toSecsSinceEpoch();
        qint64 lastModifiedTime = info.lastModified().toSecsSinceEpoch();
        qint64 lastReadTime = info.lastRead().toSecsSinceEpoch();
        package->write((const char*)&birthTime, sizeof(birthTime));
        package->write((const char*)&lastModifiedTime, sizeof(lastModifiedTime));
        package->write((const char*)&lastReadTime, sizeof(lastReadTime));
        //写权限
        bool isWriteAble = info.isWritable();
        if(isWriteAble)
        {
            int W = 1;
            package->write((const char*)&W, sizeof(W));
        }
        else
        {
            int W = 0;
            package->write((const char*)&W, sizeof(W));
        }
        //写属主
        qt_ntfs_permission_lookup++; // turn checking on
        QString owner = info.owner();
        qDebug()<<owner;
        int ownerLength = strlen(owner.toStdString().c_str());
        package->write((const char*)&ownerLength, sizeof(ownerLength));
        package->write(owner.toStdString().c_str());
        qt_ntfs_permission_lookup--; // turn it off again
        //写文件大小
        int fileLength = info.size();
        package->write((const char*)&fileLength, sizeof(fileLength));
        //写文件
        QFile data(info.absoluteFilePath());
        if(!data.open(QFile::ReadOnly))
        {
            qDebug() << "package: source file open falied" << endl;
        }
        package->write(data.readAll());
        data.close();
    }

    /*
     * fileList 备份文件列表
     * des      包名
     * 打包文件创建失败返回1
     * 源文件打开失败返回2
     * 正常退出返回0
     */
    static int pack(QStringList fileList, QString packageName)
    {
        QFile package(packageName);
        if(!package.open(QFile::WriteOnly))
        {
            qDebug() << "package: package create falied" << endl;
            return 1;
        }
        //获取备份根目录
        QString root = QFileInfo(fileList[0]).path();
        for (auto& file : fileList)
        {
            QFileInfo info = QFileInfo(file);
            //文件处理
            if(info.isFile())
            {
                //快捷方式处理
                if(info.isShortcut())
                {
                    shortCutTackle(&package, info, root);
                }
                //普通文件处理
                else
                {
                    commomFileTackle(&package, info, root);
                }
            }
            //目录处理
            else
            {
                QDirIterator iter(file, QDirIterator::Subdirectories);
                while (iter.hasNext())
                {
                    iter.next();
                    QFileInfo info = iter.fileInfo();
                    if(info.isFile())
                    {
                        //快捷方式处理
                        if(info.isShortcut())
                        {
                            shortCutTackle(&package, info, root);
                        }
                        //普通文件处理
                        else
                        {
                            commomFileTackle(&package, info, root);
                        }
                    }
                }
            }
        }
        package.close();
        return 0;
    }

};
#endif // PACKAGE_H
