#include "mythread.h"


MyThread::MyThread(qintptr ID, QObject*parent):QThread(parent)
{
    this->socketDescriptor = ID;//进行套接字绑定
}
void MyThread::run(){
    rsaToString("0x7a796a313233313233");
//初始化，下面两条语句只是测试性地初始化，实际初始化的时候需要根据自己当前服务器的信息进行调整
    servername = "as";
    despasswd = "12345678";
    //一个信号与槽，提醒线程已经收到了文件
    //采用多线程进行具体的连接的处理
    socket = new QTcpSocket;
    isRecving = false;
//    rsaEncode("client","client");
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        sendtoM(socket->errorString());
        return;
    }
    //信号于槽的绑定，使此线程能够独立地进行服务
    connect(socket, SIGNAL(readyRead()), this, SLOT(buf_recv()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    current_ip = socket->peerAddress().toString()+QString(":%1").arg(socket->peerPort());
    QString msg = "新的线程启动，服务 "+current_ip;
    sendtoM(msg);
    exec();
}

// 处理函数
void MyThread::service(QString filename){
    // 代表命令行文件
    if(filename.contains(".txt")){
        qDebug()<<filename;
        if(filename.contains(".ls"))//说明是des加密的文件，那么需要调用des解密的接口函数
        {
            QString mes = "des deconfing: ";
            qDebug()<<desDecode(filename,despasswd);
            sendtoM(mes);
            filename = filename.left(filename.size()-3);//读取文件中的内容
            QFile f = QFile(filename);
            f.open(QIODevice::ReadOnly|QIODevice::Text);
            while(!f.atEnd())
            {
                QString str =QString(f.readLine());
                qDebug()<<str;

            }
            desEncode("/home/zyj/des_response.txt", despasswd);
            sendfile("/home/zyj/des_response.txt.ls");//向客户端回复文件

        }
        else if(filename.contains(".rsa"))//说明是通过rsa算法加密传输的授信文件
            //需要读取此文件，经过数据库查询操作后返回相关的信息
        {
            //用自己的私钥解开文件
            rsaDecode(filename);
            //将解开的文件转换为字符串
            rsaToString(filename);
            QFile f = QFile(filename);
            f.open(QIODevice::ReadOnly|QIODevice::Text);//打开文件并读取
            while(!f.atEnd())
            {
                QString str = QString(f.readLine());
                qDebug()<<str;
            }
            rsaEncode("/home/zyj/rsa_response.txt.rsa", "client");
            sendfile("/home/zyj/rsa_response.txt.rsa");


        }
    }
    // 代表发送的一般文件，直接对文件进行解密后，将文件名存入对应用户的数据库中
    else{
        desDecode(info.filePath(), despasswd);
        return;
    }

}


void MyThread::sendfile(QString filename){//发送信息的接口函数
    send_File(filename, sendHead);
}


// 服务处理函数，这是线程的主要部分。
//是一个准备开始服务的程序，此程序的作用是：
/*
 * 1. 当有报文到达的时候，此程序先行读取对方传来的代表命令的语句
 * 2. 根据这些语句，此程序跳转到不同的程序进行相应的数据传输操作
*/
void MyThread::buf_recv()
{
    if(isRecving == false){
        QByteArray buf = socket->readLine();
        qDebug()<<buf;
        if(QString(buf).contains("disconnect")){
           // disconnected();
        }
        //对于文件收发逻辑的if else语句
        else if(QString(buf).contains("Head recv"))
            // 文件头部发送成功，开始发送文件的主体内容部分
            send_File("",SERV::sendFile);
        else if(QString(buf).contains("recv fin")){
            send_File("",SERV::send_fin);
        }
        //当已经接收到此指令的时候，代表对方已经发送了文件
        else if(QString(buf).contains("Head send")){
            recv_File(SERV::recvHead);
            isRecving = true;
        }
        else if(QString(buf).contains("send fin"))
            recv_File(recv_fin);
        else if(QString(buf).contains("resend")||QString(buf).contains("recvHead error")){//重新发送文件，此时代表文件出错
            file.close();
            send_File(info.filePath(), sendHead);
        }
    }
    else{
        recv_File(SERV::recvFile);
    }
}
//加解密算法的逻辑函数
bool MyThread::desEncode(QString filepath, QString passwd){
    DES des;//调用DES类实现DES文件加密
    return des.FileEncryption(filepath, passwd, ECB, false);//返回加密的结果
}
bool MyThread::desDecode(QString filepath, QString passwd){
    DES des;//调用DES类实现DES文件加密
    return des.FileDecryption(filepath, passwd, ECB, true);//返回加密的结果
}
QString MyThread::desStrEncode(QString str, QString passwd)
{
    DES des;
    return des.StrEncryption(str, passwd);
}
QString MyThread::desStrDecode(QString str, QString passwd)
{
    DES des;
    return des.StrDecryption(str, passwd);
}
bool MyThread::rsaEncode(QString filePath, QString key){
    QProcess p;//用于运行rsa加密程序
    QString cmd = "python rsa.py --e "+filePath+" "+key+".pub";
    int status =  QProcess::execute(cmd);
    if(status == 0)      return true;
        else return false;

}
bool MyThread::rsaDecode(QString filePath){
    QProcess p;
    QString cmd = "python rsa.py --d "+filePath+" "+servername+".pri";
    int status = QProcess::execute(cmd);
    if(status == 0)
        return true;
    else
        return false;
}
QString MyThread::rsaToString(QString hex){
    QProcess p;
    QString cmd = "python toString.py "+hex;
    p.start(cmd);
    p.waitForFinished();
    QString str = p.readAllStandardOutput();
    str = str.simplified();
    return str;
}
// 收发结束，将此状态写入日志、断开连接和删除套接字
void MyThread::disconnected(){
    QString mes = "服务于： " + current_ip + "的线程结束";
    sendtoM(mes);
    socket->disconnectFromHost();
    socket->close();
    socket->deleteLater();
    exit(0);
}
void MyThread::sendtoM(QString msg){
    QString date = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh-mm-ss] ");
    date.append(msg);
    emit Status(date);
}
