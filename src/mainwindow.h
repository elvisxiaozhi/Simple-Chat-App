#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Connection *connection;
    QWidget *statusWidget;
    QString localHostName;

    void setWindowLayout();
    void setOfflineStatusBar();

private slots:
    void connected();
    void unconnected();
    void sendMessage();
    void readMessage();
};

#endif // MAINWINDOW_H
