#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <iostream>
#include <fstream>
#include <QDebug>
#include <map>
#include <QMessageBox>
#include <QFileInfo>
#include "md5.h"
#include <queue>
#include <vector>
#include <bitset>

using namespace std;
class Decompress
{
    //哈夫曼树节点
    struct haffNode
    {
        unsigned long long frequency;
        unsigned char uchar;
        string code;
        struct haffNode* left = 0;
        struct haffNode* right = 0;
    };

    //比较哈夫曼节点频率
    struct cmp {
        bool operator ()(const haffNode* a, const haffNode* b) {
            return a->frequency > b->frequency;
        }
    };

    void insert_node(haffNode* father, unsigned char uchar, string code) {
        if (code.empty()) {
            father->uchar = uchar;
            return;
        }
        char way = code[0];
        if (way == '0') {
            if (!(father->left)) {
                haffNode* son = new (haffNode);
                father->left = son;
            }
            insert_node(father->left, uchar, code.substr(1));
        } else {
            if (!(father->right)) {
                haffNode* son = new (haffNode);
                father->right = son;
            }
            insert_node(father->right, uchar, code.substr(1));
        }
    }

public:
    /*
     * srcPath  文件路径
     * desPath  输出路径
     * pwd      密码
     * 0        正常退出
     * 1        源文件打开失败
     * 2        密码错误
     * 3        目标文件创建失败
     * 4        词频表缺失
     * 5        解码失败
     * 6        结尾缺失
     */
    int decompress(string srcPath, string desPath, string pwd)
    {
        ifstream ifs;
        ifs.open(srcPath, ios::in | ios::binary);
        if(!ifs)
        {
            qDebug() << "decompress: source file open failed" << endl;
            return 1;
        }

        //密码校验
        unsigned char uchar;
        int pwd_len = pwd.length();
        ifs.read((char*)&uchar, sizeof(char));
        int zeroNum = uchar;
        //标志位大于7则有密码
        if(zeroNum >= 8)
        {
            zeroNum -= 8;
            char checkMD5_c[33];
            ifs.get(checkMD5_c, 33);
            string checkMD5 = string(checkMD5_c, 32);
            string pwdMD5 = MD5().getMD5(pwd);
            if(pwdMD5 != checkMD5)
            {
                ifs.close();
                qDebug() << "decompress: wrong password" << endl;
                return 2;
            }
        }

        //创建目标文件
        ofstream ofs;
        string fileName = QFileInfo(QString::fromStdString(srcPath)).fileName().toStdString();
        string newFileName = desPath + fileName.substr(0, fileName.find_last_of(".")) + ".tar";
//        qDebug() << QString::fromStdString(newFileName) << endl;
        ofs.open(newFileName, ios::out | ios::binary);
        if(!ofs)
        {
            qDebug() << "decompress: target file create failed" << endl;
            return 3;
        }

        //读词频表
        unsigned long long freq;
        map<unsigned char, unsigned long long> freqMap;
        int i = 0;
        for (i = 0; i < 256; i++) {
            ifs.read((char*)&freq, sizeof(freq));
            if (freq) {
                freqMap[i] = freq;
            }
        }
        if (i != 256)
        {
            qDebug() << "decompress: frequency map lost" << endl;
            return 4;
        }

        //建立词频小顶堆
        priority_queue<haffNode*, vector<haffNode*>, cmp> freqHeap;
        map<unsigned char, unsigned long long>::reverse_iterator it;
        for (it = freqMap.rbegin(); it != freqMap.rend(); it++) {
            haffNode* pn = new (haffNode);
            pn->uchar = it->first;
            pn->frequency = it->second;
            pn->left = pn->right = 0;
            freqHeap.push(pn);
        }

        //构建哈夫曼树
        while (freqHeap.size() > 1) {
            haffNode* pn1 = freqHeap.top();
            freqHeap.pop();
            haffNode* pn2 = freqHeap.top();
            freqHeap.pop();
            haffNode* pn = new (haffNode);
            pn->frequency = pn1->frequency + pn2->frequency;
            pn->left = pn1;
            pn->right = pn2;
            freqHeap.push(pn);
        }
        haffNode* root = freqHeap.top();

        //解码
        haffNode* decodePointer = root;
        //buf指向now后一个字节，now是正在处理的字节
        string buf, now;
        ifs.read((char*)&uchar, sizeof(unsigned char));
        int pwd_index = 0;
        if (pwd_len) {
            uchar ^= pwd[pwd_index++];
            pwd_index %= pwd_len;
        }
        bitset<8> bs = uchar;
        buf = bs.to_string();
        while (ifs.read((char*)&uchar, sizeof(unsigned char))) {
            if (pwd_len) {
                uchar ^= pwd[pwd_index++];
                pwd_index %= pwd_len;
            }
            bitset<8> bs = uchar;
            now = buf;
            buf = bs.to_string();
            for (char i = 0; i < 8; i++) {
                if (now[i] == '0') {
                    if (!decodePointer->left)
                    {
                        qDebug() << "decompress: decode failed" << endl;
                        return 5; // 解码错误
                    }
                    decodePointer = decodePointer->left;
                } else {
                    if (!decodePointer->right)
                    {
                        qDebug() << "decompress: decode failed" << endl;
                        return 5; // 解码错误
                    }
                    decodePointer = decodePointer->right;
                }
                if (!(decodePointer->left || decodePointer->right)) {
                    ofs.write((char*) & (decodePointer->uchar), sizeof(decodePointer->uchar));
                    decodePointer = root; //回到哈夫曼树的根
                }
            }
        }
        //处理最后一个字节
        now = buf;
        for (char i = 0; i < (8 - zeroNum) % 8; i++) {
            if (now[i] == '0') {
                if (!decodePointer->left)
                {
                    qDebug() << "decompress: decode failed" << endl;
                    return 5; // 解码错误
                }
                decodePointer = decodePointer->left;
            } else {
                if (!decodePointer->right)
                {
                    qDebug() << "decompress: decode failed" << endl;
                    return 5; // 解码错误
                }
                decodePointer = decodePointer->right;
            }
            if (!(decodePointer->left || decodePointer->right)) {
                ofs.write((char*) & (decodePointer->uchar), sizeof(unsigned char));
                decodePointer = root;
            }
        }
        ifs.close();
        ofs.close();
        if (decodePointer == root) return 0; // 正常执行
        return 6; // 文件结尾不完整
    }
};

#endif // DECOMPRESS_H
