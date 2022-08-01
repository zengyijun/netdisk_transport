#include"serv_client.h"
//底层的收发函数实现

bool serv_client::recv_File(int mode){
    QByteArray buf = socket->readAll();
    switch(mode){
    case recvHead:{
        file.close();
        fileName.clear();//清除其中原有内容
        fileName = QString(buf).section("##", 0, 0);
        fileSize = QString(buf).section("##", 1, 1).toInt();
        recvSize = 0;//初始时没有收到任何信息
        info = QFileInfo("./clientFile/"+fileName);
        file.setFileName(info.filePath());
        if(!file.open(QIODevice::WriteOnly|QIODevice::Append)){
            socket->write("recvHead error\n");
            file.close();
            return false;
        }
        isRecving = true;
        socket->write("Head recv\n");
        break;
    }
        // 对文件本身进行接收
    case recvFile:{
        qint64 len = file.write(buf);
        recvSize += len;
        if(recvSize == fileSize){
            file.close();
            socket->write("recv fin\n");
            isRecving = false;
        }
        break;
    }
        //当文件接收结束的时候接收一个md5值，用于文件校验
    case recv_fin:{
        file.close();
        // 接收结束，计算文件的md5值。如果和客户端传来的一致则不管，否则要求客户端重发
        QString md5 = QString(buf).simplified();
        md5 = rsaDe_sig(md5, "as");
        md5 = rsaToString(md5).remove("md5:");
        QFile f(info.filePath());
        f.open(QIODevice::ReadOnly);
        QByteArray ba = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);//计算接收到的文件的md5值
        f.close();
        if(md5 == ba.toHex().constData()){//md5值检测成功
            //调用信号函数，向上层类反映收到了文件
            emit recv(info.filePath());
            return true;
        }
        else{
            socket->write("resend\n");//让服务器重新传送当前文件，文件传输中出错了
            QFile::remove(info.filePath());//删除收到的文件
            return false;
        }
        break;
    }
    }
    return true;
}
bool serv_client::send_File(QString filepath, int mode){
    switch(mode){
    case sendHead:{

        socket->write("Head send\n");
        file.close();
        //发送文件头的部分
        // 打开传入的filePath指示的文件内容，并将这些内容赋值到全局变量
        if(!filepath.isEmpty()){
            fileName.clear();
            info = QFileInfo(filepath);
            fileName = info.fileName();
            fileSize = info.size();
            sendSize = 0;
        }
        filepath.toLocal8Bit().constData();
        file.setFileName(filepath);
        if(!file.open(QIODevice::ReadOnly)){
            file.close();
        }
        // 接下来构造发送的头部信息：
        QString head = QString("%1##%2").arg(fileName).arg(fileSize);
        qint64 len = socket->write(head.toUtf8());
        //判断文件是否发送成功
        if(len < 0){
            file.close();
            return false;//发送失败，返回false
        }
        break;
    }
    case sendFile:{
        senddata();
        break;
    }
        //向对方发送本文件的md5值，以便对方校验自己所收到的文件是否有误
    case send_fin:{
        file.close();
        socket->write("send fin\n");//提醒用户发送结束，现在正在发送的是文件的md5值
        QFile f = QFile(info.filePath());//打开文件
        f.open(QIODevice::ReadOnly);
        QByteArray ba = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);//计算接收到的文件的md5值
        QString mes = "md5:";
        mes.append(ba.toHex().constData());
        mes = rsaEn_sig(mes);
        qDebug()<<"<<<<<<<<<<<<"<<mes;
        socket->write(mes.toUtf8());//发送md5信息
    }
    }
    return true;

}
void serv_client::senddata()//正式的循环发送文件模块
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
