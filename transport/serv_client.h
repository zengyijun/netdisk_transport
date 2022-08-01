#ifndef SERV_CLIENT_H
#define SERV_CLIENT_H
#include<QNetworkProxy>
#include<QTcpSocket>
#include<QFileInfo>
#include<QFile>
#include<QString>
#include<QProcess>
#include<QCryptographicHash>//计算文件的哈希值，如果不对则要求对方重传
#include"des.h"

#define BUF_SIZE 4*1024

/* client的服务类，用于：
 * 1. 提供客户端与三台服务器进行连接的接口
 * 2. 接收来自服务器的数据，并完成相应文件的收发*/

class serv_client:public QObject
{
    Q_OBJECT
public:
    // 这些枚举变量既用于文件的收发，也用于客户端与服务器之间的同步
    enum client{
      //用于发送的枚举型变量
      sendHead = 1,
      sendFile,
      send_fin,

      //用于文件接收的枚举变量
        recvHead = 100,
        recvFile,
        recv_fin,

        AS = 1000,
        TGS,
        SERVER
    };
    serv_client();//构造函数，调用时需要给出本客户端的名字
    ~serv_client();//析构函数
    void setclientname(QString name);
    bool sendfile(QFileInfo file);//一个接口函数，让服务函数能够调用此函数完成文件的发送
    bool con_s(int mode);//用于服务器的选择性连接
    //加密函数，在文件发送前/文件接收后调用
    void setDespasswd(QString passwd);//设置与服务器通信时所用的des加密的密钥
    //des加密：对文件进行加密
    bool desEncode(QFileInfo filepath);//des加密
    bool desDecode(QFileInfo filepath);//des解密
    //des加密：对某一些字符串进行des加密
    QString desStrEncode(QString str);//des对一个字符串直接加密
    QString desStrDecode(QString str);//des对一个字符串直接解密
    //rsa加密，对认证文件进行加密
    bool rsaEncode(QFileInfo filepath, QString pub_key);//rsa加密,需要人为地指定公钥
    bool rsaDecode(QFileInfo filepath);//用自己的私钥解密
    //rsa解密：将解密后的文件转换成字符串
    QString rsaToString(QString hex);//将文件中的信息从数字转换成字符串
    //rsa加密，对数字签名进行加密
    QString rsaDe_sig(QString str, QString serv);
    QString rsaEn_sig(QString str);
    //信号函数，在正确地接收到文件的时候
signals:
    void recv(QString filename);//告诉上层收到了文件

public slots:
    void buf_recv();//当socket中有新的字节的字到达的时候

private:
    bool recv_File(int type);//接收文件
    bool send_File(QString filename, int type);//发送文件的私有服务函数
    void senddata();//循环处理函数，用于循环地发
private:
    // 基础的全局变量
    QTcpSocket *socket;//套接字，用于通信
    QString current_name;//指定当前客户端的名称，便于rsa解密的过程（能够确定私钥）
    QString despasswd;//给出des加密算法的密钥

    //以下变量服务于文件收发
    QFile file;
    bool isRecving;//判断当前是否正在接收文件，如果是则调整槽函数的响应状态
    QFileInfo info;
    QString fileName;//用于接收时确保文件名称正确
    qint64 fileSize;//文件本身的大小，以便于判断是否接收完成
    qint64 sendSize;//代表已发送的大小
    qint64 recvSize;//代表此时已经接收了文件的大小
};

#endif // SERV_CLIENT_H
