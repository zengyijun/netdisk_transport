#ifndef SERVER_H
#define SERVER_H
#include"mythread.h"
#include<QTcpServer>
#include<QTcpSocket>
#include<QString>
#include<QDateTime>

class server:public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QObject *parent = 0);
    void startServer();
public slots:
    void callStatus(QString msg);//这个信号与槽和副线程绑定，将副线程相关的信息输出到主线程中
signals:
    void sendStatus(QString & msg);
protected:
    void incomingConnection(qintptr socketDescriptor);
private:
    void sendSig(QString msg);

};

#endif // SERVER_H
