#include "server.h"
#include"serverform.h"
server::server(QObject *parent): QTcpServer(parent)
{
}
void server::startServer()
{
    int port = 8888;
    QString msg;
    if(!this->listen(QHostAddress::Any, port))
    {
        msg = "Could not start server";
        sendSig(msg);
    }
    else msg = QString( "successfully start server, listening on port %1").arg(port);
    sendSig(msg);
}
void server::incomingConnection(qintptr socketDescriptor)
{
    MyThread* thread = new MyThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()),thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(Status(QString)),this ,SLOT(callStatus(QString)));
    thread->start();
}
void server::sendSig(QString msg)
{
    // 打印true
    QString date = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss] ");
    date.append(msg);
    emit sendStatus(date);
}
void server::callStatus(QString msg){
    QString date = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss] ");
    date.append(msg);
    emit sendStatus(date);
}
