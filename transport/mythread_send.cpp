#include "mythread.h"
//用来写收发逻辑函数
void MyThread::recv_File(int type)//接收文件
{
    QString mes = "receive file from: "+ current_ip;
    QByteArray buf = socket->readAll();
    if(type == SERV::recvHead){
        //接收包头
        info.refresh();
        fileName.clear();
        fileName = QString(buf).section("##", 0, 0);
        mes += ".its file name is ";
        mes.append(fileName);
        fileSize = QString(buf).section("##", 1, 1).toInt();
        mes += QString(" size is %1 %2").arg(fileSize/1000).arg("kbit");
        recvSize = 0;//初始时没有收到任何信息
        sendtoM(mes);
        info = QFileInfo("./serverFile/"+fileName);
        file.setFileName(info.filePath());
        if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
            sendtoM("receive file from "+current_ip+fileName+" : faild");
        socket->write("Head recv\n");
    }
    //用于处理接收消息
    else if(type == SERV::recvFile)
    {
        qDebug()<<"recv";
        //开始接收文件的过程
        qint64 len = file.write(buf);
        recvSize += len;
        if(recvSize == fileSize)
        {
            file.close();
            mes += " recv succeed, checking md5";
            sendtoM(mes);
            socket->write("recv fin\n");
            isRecving = false;

        }
    }
    //对方发送结束，此处用以检查文件的md5值
    else if(type == recv_fin){
        QString md5 = QString(buf).remove("md5:");
        QFile f(info.filePath());
        f.open(QIODevice::ReadOnly);
        QByteArray ba = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);//计算接收到的文件的md5值
        if(md5 == ba.toHex().constData()){
            //调用信号函数，向上层类反映收到了文件
            mes+="md5 is right, file is now saved!";
            sendtoM(mes);
            service(info.filePath());
        }
        else{
            socket->write("resend\n");//让服务器重新传送当前文件，文件传输中出错了
            QFile::remove(info.filePath());//删除收到的文件
            mes.clear();
            mes = "recving "+ info.fileName()+" from "+current_ip+" faild, the md5 number is wrong.";
            mes += "already ask for resend";
            sendtoM(mes);//文件接收错误，请求让对方重发
        }
    }
}
void MyThread::send_File(QString filePath, int type){
    // 输出到日志文件的信息
    QString mes;
    switch(type){
    case SERV::sendHead:{
            socket->write("Head send\n");
        mes = "open files for sending to "+current_ip;
        // 打开传入的filePath指示的文件内容，并将这些内容赋值到全局变量
        if(!filePath.isEmpty()){
            fileName.clear();
            info.refresh();
            info = QFileInfo(filePath);
            fileName = info.fileName();
            fileSize = info.size();
            sendSize = 0;
        }
        filePath.toLocal8Bit().constData();
        file.setFileName(filePath);
        if(!file.open(QIODevice::ReadOnly)){
            mes += " :faild, file can't open";
            sendtoM(mes);
//            socket->write("open error\n");//文件打开错误
            sendfile(info.filePath());
        }
        mes+= " successfully opened file";
        sendtoM(mes);
        mes.clear();
        mes = "sending file head to "+current_ip;
        // 接下来构造发送的头部信息：
        QString head = QString("%1##%2").arg(fileName).arg(fileSize);
        qint64 len = socket->write(head.toUtf8());
        //判断文件是否发送成功
        if(len < 0){
            mes += " :faild, can't send file head";
            sendtoM(mes);
            file.close();
        }
        sendtoM(mes+" :succeed");
        break;}
    case SERV::sendFile:{
        mes = "sending file itself to client "+current_ip;
        senddata();
        mes += " succeed";
        sendtoM(mes);
        break;
    }
    case send_fin:{
        //发送结束
        //此时发送文件的md5值
        socket->write("send fin\n");
        file.close();
        QFile f = QFile(info.filePath());
        f.open(QIODevice::ReadOnly);
        QByteArray ba = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);//计算接收到的文件的md5值
        QString md5 = "md5:";
        md5.append(ba.toHex().constData());
        socket->write(md5.toUtf8());//发送md5信息

    }
    }
}


void MyThread::senddata()//正式的循环发送文件模块
{
    qint64 len = 0;
    do{
    char buf[BUF_SIZE] = {0};
    len = 0;
    len = file.read(buf, BUF_SIZE);
    len = socket->write(buf, len);
    sendSize += len;
    }while(len > 0);
}
