#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtNetwork>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

    QTcpSocket *tcpSocket;
    void showUnconnectedMsgBox();

signals:
    void connectionSucceeded();
    void unconnectedState();
    void showError(QAbstractSocket::SocketError);

public slots:
    void connectToServer();
    void sendMessages(QString);
    void getError(QAbstractSocket::SocketError);
};

#endif // CONNECTION_H
