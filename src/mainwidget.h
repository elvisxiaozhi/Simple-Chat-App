#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "socket.h"

namespace Ui {
class MainWidget;
}

class QListWidgetItem;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    Socket *socket;
    QVector<QListWidgetItem *> userVec;
    QVector<QString> userIDVec;

    void setWindowLayout();
    void setSocket();
    void addToUserVec(QString);

private slots:
    void connected();
    void unconnected();
    void statusChanged(int);
    void readMessage();
    void userListDoubleClicked(QListWidgetItem *);
};

#endif // MAINWIDGET_H
