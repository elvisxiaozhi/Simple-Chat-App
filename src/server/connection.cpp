#include "connection.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QDebug>

Connection::Connection(QObject *parent) : QTcpServer(parent)
{
    this->listen(QHostAddress::Any, 6666);

    connect(this, &Connection::newConnection, this, &Connection::clientConnected);
}

Connection::~Connection()
{
}

void Connection::incomingConnection(qintptr socketDescriptor)
{
    SocketThread *socketThread = new SocketThread(socketDescriptor);

    threadVec.push_back(socketThread);

    connect(socketThread, &SocketThread::started, [&]() { emit socketThread->messageToClient("Hello"); });
    connect(socketThread, &SocketThread::finished, [&, socketDescriptor]() {
        for(int i = 0; i < threadVec.size(); ++i) {
            if(threadVec[i]->socketDescriptor == socketDescriptor) {
                threadVec[i]->deleteLater();
                threadVec.erase(threadVec.begin() + i);
                qDebug() << "Client" << socketDescriptor << "disconnected.";
            }
        }
    });
    qDebug() << "Client" << socketDescriptor << "connected.";
}

void Connection::clientConnected()
{
    QString message = "Connected clients: ";
    for(int i = 0; i < threadVec.size(); ++i) {
        message.append(QString::number(threadVec[i]->socketDescriptor) + " ");
    }
}
