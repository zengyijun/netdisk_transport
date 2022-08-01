#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    clientform *new_client = new clientform;
    new_client->show();
    this->close();
}


void MainWindow::on_pushButton_2_clicked()
{
    serverform *new_server = new serverform;
    new_server->show();
    this->close();
}

