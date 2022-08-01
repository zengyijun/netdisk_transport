#pragma once
#ifndef MYTHREAD_H
#define MYTHREAD_H

// 此多线程的作用是是实现文件的收发
#include <QThread>//多线程
#include<QProcess>
#include<QTcpSocket>
#include<QFile>
#include<QString>
#include <QTimer>
#include<QDateTime>
#include <QHostAddress>
#include<QDebug>
#include<QCryptographicHash>
#include<QFileInfo>
#include"des.h"
#define BUF_SIZE 1024*4

class MyThread : public QThread
{
    Q_OBJECT

public:
    explicit MyThread(qintptr ID, QObject *parent = 0);
    void run();
    // 两种加密算法对文件进行加解密
    bool desEncode(QString filepath, QString passwd);
    bool desDecode(QString filepath, QString passwd);
    QString desStrEncode(QString str, QString passwd);
    QString desStrDecode(QString str, QString passwd);
    bool rsaEncode(QString filepath, QString pub_key);//输入要进行rsa加密的文件、所使用的公钥文件名称
    bool rsaDecode(QString filepath);
    //rsa加密文件的公钥的文件名为rsa.pub；私钥的文件名为rsa.pri。当要传输
    //公钥文件的时候，最好是把文件名的前面加上自己的主机名（自定义的一个名称），以防出错
    bool rsaCreate(QString path);//一个函数,用于创建rsa算法的公钥与私钥。需要显式地指定生成的目录，且需要保证相应目录一定存在
    QString rsaToString(QString hex);//将文件中的内容转换为String形式，以供阅读
    QString rsaDe_sig(QString str);
        void service(QString filename);
    void sendfile(QString filename);//sendfile的接口函数
signals:
    void error(QTcpSocket::SocketError socketerror);
    //信号与槽
//    void recv_(QString filename);//当文件接收正确时调用此信号，通知相关函数/类有新的文件收到（这样写是错误的，因为信号与槽的实现机制是多线程
    void Status(QString msg);
public slots:
    void buf_recv();

    void disconnected();
private:
    void sendtoM(QString mes);//将信息发送给主进程
    void senddata();
    void send_File(QString Filepath, int type);//发送特定目录下的文件
    void recv_File(int type);//接收文件
//    void notify(QString filename);//告诉相关槽函数相关信息
private:
    bool isRecving;
    QTcpSocket *socket;
    qintptr socketDescriptor;

    QFile file;//用于打开文件
    QString fileName;//文件名字
    qint64 fileSize;//文件本身的大小
    qint64 sendSize;//发送文件时已经发送文件的大小
    qint64 recvSize;//接收文件时已经接收文件的大小

    QString current_ip;//保存当前线程服务的ip地址
    QString servername;//保存当前的服务器类型名称，用于rsa解密的时候文件定位
    QString despasswd;//des加密的密钥
    QFileInfo info;

    //局部枚举变量的定义
    enum SERV{
        //这些枚举变量是为发送信息服务的
        sendHead = 1, // 一些枚举变量表示状态信息，sendhead表示发送文件头
        sendFile, //表示发送文件本体
        send_fin,//表示发送已经结束

        //这些枚举变量代表接收消息
        recvHead = 10, //表示接收到了文件头，进一步进行接收
        recvFile,
        recv_fin
    };
};

#endif // MYTHREAD_H
