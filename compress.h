#ifndef COMPRESS_H
#define COMPRESS_H

#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <vector>
#include <bitset>
#include <QDebug>
#include <tchar.h>
#include <QDir>
#include<QTextCodec>
#include "md5.h"

using namespace std;
class Compress
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

    //编码表
    map<unsigned char, string> codeMap;

    //比较哈夫曼节点频率
    struct cmp {
        bool operator ()(const haffNode* a, const haffNode* b) {
            return a->frequency > b->frequency;
        }
    };

    void encode(haffNode* pn, string code) {
        pn->code = code;
        if (pn->left) encode(pn->left, code + "0");
        if (pn->right) encode(pn->right, code + "1");
        if (!pn->left && !pn->right) {
            codeMap[pn->uchar] = code;
        }
    }

public:
    /*
     * sourcePath 文件名
     * des      输出路径
     * pwd      密码
     * 1        打包文件创建失败返回
     * 2        源文件打开失败返回2
     * 0        正常退出返回0
     */
    int compress(string sourcePath, string des, string pwd)
    {
        ifstream ifs;
        ifs.open(sourcePath, ios::in | ios::binary);
        if(!ifs)
        {
            qDebug() << "compress: source file open failed" << endl;
            return 1;
        }
        ofstream ofs;
        string fileName = sourcePath.substr(0, sourcePath.find_last_of("."));
        string newfileName = des + fileName + ".bak";
        ofs.open(newfileName, ios::out | ios::binary);
        if (!ofs) {
            ifs.close();
            qDebug() << "compress: target file create failed" << endl;
            return 2;
        }

        //统计词频
        unsigned char uchar;
        map<unsigned char, unsigned long long> frequencyMap;
        while (ifs.read((char*)&uchar, sizeof(char))) {
            frequencyMap[uchar]++;
        }

        //建立词频小顶堆
        priority_queue<haffNode*, vector<haffNode*>, cmp> freqHeap;
        map<unsigned char, unsigned long long>::reverse_iterator it;
        for (it = frequencyMap.rbegin(); it != frequencyMap.rend(); it++) {
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

        //编码
        codeMap.clear();
        encode(root, "");

        //写头部：补零数+密码标志位
        const unsigned char flags = 0;
        ofs.write((char*)&flags, sizeof(flags));

        //写头部：密码
        int pwd_len = pwd.length();
        if (pwd_len) {
            string pwdMD5 = MD5().getMD5(pwd).c_str();
            ofs << pwdMD5;
        }

        //写头部：词频表
        string freqTable;
        const unsigned long long zeroULL = 0;
        for (int i = 0; i < 256; i++) {
            if (frequencyMap.count(i) == 0) {
                ofs.write((char*)&zeroULL, sizeof(zeroULL));
            } else {
                unsigned long long freq = frequencyMap[i];
                ofs.write((char*)&freq, sizeof(freq));
            }
        }

        //写文件并加密
        {
            int pwd_index = 0;
            ifs.clear();
            ifs.seekg(0);
            string buf;
            unsigned char uchar;
            while (ifs.read((char*)&uchar, sizeof(uchar))) {
                buf += codeMap[uchar];
                while (buf.length() >= 8) {
                    bitset<8> bs(buf.substr(0, 8));
                    uchar = bs.to_ulong();
                    if (pwd_len) {
                        uchar ^= pwd[pwd_index++];
                        pwd_index %= pwd_len;
                    }
                    ofs.write((char*)&uchar, sizeof(uchar));
                    buf = buf.substr(8);
                }
            }
            // 末尾处理
            int zeroNum = (8 - buf.length()) % 8;
            if (zeroNum) {
                for (int i = 0; i < zeroNum; i++) {
                    buf += "0";
                }
                bitset<8> bs(buf.substr(0, 8));
                uchar = bs.to_ulong();
                if (pwd_len) {
                    uchar ^= pwd[pwd_index++];
                    pwd_index %= pwd_len;
                }
                ofs.write((char*)&uchar, sizeof(uchar));
            }
            //写入头部预留的补零数+密码标志字段
            ofs.clear();
            ofs.seekp(0);
            if (pwd_len) {
                zeroNum += 8;
            }
            uchar = zeroNum;
            ofs.write((char*)&uchar, sizeof(uchar));
        }
        ifs.close();
        ofs.close();
        return 0;
    }
};
#endif // COMPRESS_H
