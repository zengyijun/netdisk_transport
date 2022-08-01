#include "serv_client.h"
#include "clientform.h"
//#include "clientform_ui.h"
serv_client::serv_client()
{

    isRecving = false;
    socket = new QTcpSocket();
    despasswd = "";
    connect(socket, SIGNAL(readyRead()), this, SLOT(buf_recv()), Qt::DirectConnection);//信号与槽的绑定，让socket每次检测到有报文到达都能将其转到相应的处理函数进行处理
    fileSize = sendSize = recvSize = 0;
}
void serv_client::setclientname(QString name){
    current_name = name;
}
serv_client::~serv_client()
{
    socket->deleteLater();
}

//加密的模块
// 这一个cpp文件中写的时serv_client类中的加密算法的实现
void serv_client::setDespasswd(QString passwd){despasswd = passwd;};
bool serv_client::desEncode(QFileInfo filepath)
{
    if(despasswd == "")
        return false;
    else{
        DES des;
        return des.FileEncryption(filepath.filePath(),despasswd,ECB, false);
    }
}
bool serv_client::desDecode(QFileInfo filepath){
    if(despasswd == nullptr)
        return false;
    else{
        DES des;
        return des.FileDecryption(filepath.filePath(), despasswd, ECB, true);
    }
}
QString serv_client::desStrEncode(QString str){
    DES des;
    return des.StrEncryption(str, despasswd);
}
QString serv_client::desStrDecode(QString str){
    DES des;
    return des.StrDecryption(str, despasswd);
}
bool serv_client::rsaEncode(QFileInfo filepath, QString key){
    QString cmd = "python rsa.py --e "+filepath.filePath()+" "+key+".pub";
    int status = QProcess::execute(cmd);
    if(status == 0) return true;
    else return false;
}
bool serv_client::rsaDecode(QFileInfo filepath){
    QString cmd = "python rsa.py --d "+filepath.filePath()+" "+current_name+".pri";
    int status = QProcess::execute(cmd);
    if(status == 0)
        return true;
    else
        return false;
}
QString serv_client::rsaToString(QString hex){
    QProcess p;
    QString cmd = "python toString.py "+hex;
    p.start(cmd);
    p.waitForFinished();
    QString str = p.readAllStandardOutput();
    str = str.simplified();
    return str;
}
QString serv_client::rsaEn_sig(QString str)
{
    QString cmd = "python rsa_.py --e "+str+" "+current_name+".pri";
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    return p.readAllStandardOutput();
}
QString serv_client::rsaDe_sig(QString str, QString serv)
{
    QString cmd = "python rsa_.py --e "+str+" "+serv+".pub";
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    return p.readAllStandardOutput();
}
//连接服务器的模块
bool serv_client::con_s(int mode)//用于建立与服务器之间的连接
{
    switch(mode){
    case AS:{
        socket->connectToHost("192.168.214.128", 8888);
        break;
    }
    case TGS:{
        socket->connectToHost("127.0.0.1", 9999);
        break;
    }
    case SERVER:{
        socket->connectToHost("192.168.214.128", 10000);
        break;
    }
    }
    if(!socket->waitForConnected())
        return false;
    else return true;
}
//发送文件的接口函数
bool serv_client::sendfile(QFileInfo filepath){
    return send_File(filepath.filePath(), sendHead);
}
//接收到信息时的槽函数
void serv_client::buf_recv()
{
    QByteArray arr;//用于随时读取信息并提供响应
    if(!isRecving){
        //当没有在收文件的时候，此时socket中会含有有用的同步信息
        //需要读取出来，依据这些同步信息来采取服务

        arr = socket->readLine();
        qDebug()<<QString(arr);
        if(QString(arr).contains("Head send")){
            //对方发送了头，那么客户端应该接收头
            recv_File(recvHead);
            isRecving = true;
        }
        else if(QString(arr).contains("Head recv"))
            //对方成功接收了头，那么应该发送文件本身
            send_File("", sendFile);
        else if(QString(arr).contains("recv fin"))
            send_File("", send_fin);
        //此函数用来校验send_fin后对方发送的md5值和自己计算出的md5值
        else if(QString(arr).contains("send fin"))
            recv_File(recv_fin);
        //代表对方文件接收错误。此时应删除本地的文件后重新发送
        else if(QString(arr).contains("resend")||QString(arr).contains("recvHead error")){
            file.close();
            send_File(info.filePath(), sendHead);
        }
    }
    else
        recv_File(recvFile);
}
