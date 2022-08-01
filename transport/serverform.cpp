#include "serverform.h"
#include "ui_serverform.h"

serverform::serverform(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::serverform)
{
    ui->setupUi(this);
    tcpserver = new server;
    ui->textBrowser->setPlainText("服务器日志");
    connect(tcpserver,SIGNAL(sendStatus(QString&)), this, SLOT(addStatus(QString&)));
    tcpserver->startServer();
}

serverform::~serverform()
{
    delete ui;
}
void serverform::addStatus(QString &status){
    ui->textBrowser->append(status);

}
