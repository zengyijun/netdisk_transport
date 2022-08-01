#ifndef SERVERFORM_H
#define SERVERFORM_H

#include <QMainWindow>
#include"server.h"
#include "mythread.h"


namespace Ui {
class serverform;
}

class serverform : public QMainWindow
{
    Q_OBJECT

public:
    explicit serverform(QWidget *parent = 0);
    ~serverform();
public slots:
    void addStatus(QString &status);

private:
    Ui::serverform *ui;
    server *tcpserver;
};

#endif // SERVERFORM_H
