#include "clientform.h"
#include "ui_clientform.h"

clientform::clientform(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::clientform)
{
    ui->setupUi(this);
    demon = new serv_client();
    //对demon进行初始化的操作
    demon->setDespasswd("12345678");
    demon->setclientname("client");
    // 信号与槽绑定服务函数：当接收到服务器发送的文件时采取的措施
    connect(demon, SIGNAL(recv(QString)), this, SLOT(filerecv(QString)));//信号与槽绑定

}
clientform::~clientform()
{
    delete ui;
}
// 进行服务响应，针对对方发送的字符串进行文件的收发状态的调整
// 此服务进行的只是接收服务，进行对文件接收的操作
// 此函数根据当前客户端接收文件的状态来进行相应的服务

void clientform::filerecv(QString filename){
     QMessageBox::information(this, "提醒", "收到了新文件，文件名是："+filename);
    //当接收到文件时采取的措施
    if(filename.contains(".txt")){
        if(filename.contains(".ls")){
            demon->desDecode(filename);
            QFile f = QFile(filename);
            f.open(QIODevice::ReadOnly|QIODevice::Text);
            if(!f.atEnd())
                qDebug()<<QString(f.readLine());
        }
    }
}

void clientform::on_pushButton_clicked()
{
//按钮，用于连接到服务器

    demon->con_s(serv_client::SERVER);
    demon->setclientname("client");
    demon->setDespasswd("12345678");

}


void clientform::on_pushButton_2_clicked()
{
    qDebug()<<demon->desEncode(QFileInfo("C:\\Users\\zengyijun\\Desktop\\diagram.png"));
    demon->sendfile(QFileInfo("C:\\Users\\zengyijun\\Desktop\\diagram.png.ls"));
}


void clientform::on_pushButton_3_clicked()
{

//des加密的认证文件收发测试
    QFile f = QFile("./au.txt.des");
    f.open(QIODevice::WriteOnly);
    QStringList des;
    des<<"56:0A:F2:F6:0A:B7"<<"56:0A:F2:F6:0A:B7"<<"current_time = 2022.06.05 11.08.56";
    QTextStream stream(&f);
    for(int i = 0; i < des.size(); i++)
        stream<<demon->desStrEncode(des.at(i))<<"\n";
    f.close();
    demon->sendfile(QFileInfo("./au.txt.des"));
}



void clientform::on_pushButton_4_clicked()
{
    demon->sendfile(QFileInfo("C:\\Users\\zengyijun\\Desktop\\AS.txt.rsa"));
}

