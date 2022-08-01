QT       += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientform.cpp \
    des.cpp \
    main.cpp \
    mainwindow.cpp \
    mythread.cpp \
    mythread_send.cpp \
    serv_client.cpp \
    serv_cliented.cpp \
    server.cpp \
    serverform.cpp

HEADERS += \
    clientform.h \
    des.h \
    mainwindow.h \
    mythread.h \
    serv_client.h \
    server.h \
    serverform.h

FORMS += \
    clientform.ui \
    mainwindow.ui \
    serverform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    serverFile.qrc
