#ifndef CLIENTFORM_H
#define CLIENTFORM_H
#include <QMainWindow>
#include <QTcpSocket>
#include<QFileInfo>
#include<QFile>
#include<QString>
#include<QMessageBox>
#include <QHostAddress>
#include"serv_client.h"

namespace Ui {
class clientform;
}

class clientform : public QMainWindow
{
    Q_OBJECT

public:
    explicit clientform(QWidget *parent = nullptr);

    ~clientform();
public slots:
    void filerecv(QString filename);
private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();



    void on_pushButton_4_clicked();

private:
    Ui::clientform *ui;
    serv_client *demon;//
};

#endif // CLIENTFORM_H
