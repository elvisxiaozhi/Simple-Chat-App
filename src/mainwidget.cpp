#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QHostInfo>
#include "chatwindow.h"
#include <QSound>
#include <QTime>
#include <QDir>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setWindowLayout();
    setSocket();
}

MainWidget::~MainWidget()
{
    //remove chat history when app is closed;
    QDir dir(ChatWindow::dataPath);
    dir.removeRecursively();
    delete ui;
}

void MainWidget::setWindowLayout()
{
    setWindowTitle("Chatty");

    ui->userLbl->setText(QHostInfo::localHostName());

    ui->statusBox->addItem(tr("Online"));
    ui->statusBox->addItem(tr("Offline"));

    connect(ui->statusBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWidget::statusChanged);
    connect(ui->userList, &QListWidget::itemDoubleClicked, this, &MainWidget::userListDoubleClicked);
}

void MainWidget::setSocket()
{
    socket = new Socket(this);

    connect(socket, &Socket::connected, this, &MainWidget::connected);
    connect(socket, &Socket::unconnected, this, &MainWidget::unconnected);
    connect(socket, &Socket::readyRead, this, &MainWidget::readMessage);

    socket->connectToServer();
}

void MainWidget::addToUserVec(QString username)
{
    QListWidgetItem *item = new QListWidgetItem(ui->userList);

    item->setText(username);

    userVec.push_back(item);

    ui->userList->addItem(item);
}

void MainWidget::recieveSocket(QString message)
{
    QStringList onlineUsers = message.split(" NextSocket: ");
    onlineUsers.pop_back();
    for(int i = 0; i < onlineUsers.size(); ++i) {
        if(std::find(userIDVec.begin(), userIDVec.end(), onlineUsers[i].split(" ")[1]) != userIDVec.end()) {
            ;
        }
        else {
            addToUserVec(onlineUsers[i].split(" ")[0]);
            userIDVec.push_back(onlineUsers[i].split(" ")[1]);
        }
    }
}

void MainWidget::socketDisconnected(QString message)
{
    QString socketID = message.split("disconnectedSocket: ")[1];
    int pos = std::find(userIDVec.begin(), userIDVec.end(), socketID) - userIDVec.begin();
    userIDVec.erase(userIDVec.begin() + pos);
    delete userVec[pos];
    userVec.erase(userVec.begin() + pos);
}

void MainWidget::recieveMessage(QString message)
{
    QStringList stringList = message.split("messageFrom: ");
    int pos = std::find(userIDVec.begin(), userIDVec.end(), stringList[1].split(" hereAreMessages: ")[0]) - userIDVec.begin();
    userVec[pos]->setTextColor(Qt::red);

    QSound::play(":/Sounds/notification.wav");

    QString currentTime = QTime::currentTime().toString("h:mm:ss AP");
    QString msColor = "<font color = \"green\">";

    if(hasChatWindow(userIDVec[pos].toInt())) {
        chatWindowVec[pos]->recieveMessage(msColor + currentTime + "<br>", msColor + stringList[1].split(" hereAreMessages: ")[1] + "<br>");
    }
    else {
        ChatWindow::saveChatHistory(userIDVec[pos], msColor + currentTime + "<br>", msColor + stringList[1].split(" hereAreMessages: ")[1] + "<br>");
    }
}

void MainWidget::createChatWindow(QListWidgetItem *item)
{
    ChatWindow *chatWindow = new ChatWindow(0, item->text(), userIDVec[ui->userList->currentRow()].toInt());
    chatWindow->readChatHistory(userIDVec[ui->userList->currentRow()]);

    chatWindowVec.push_back(chatWindow);

    connect(chatWindow, &ChatWindow::messageToWrite, [this](QString message){ socket->write(message.toUtf8()); });
    connect(chatWindow, &ChatWindow::closingWindow, [&](int socketID){
        for(int i = 0; i < chatWindowVec.size(); ++i) {
            if(chatWindowVec[i]->socketID == socketID) {
                chatWindowVec[i]->deleteLater();
                chatWindowVec.erase(chatWindowVec.begin() + i);
            }
        }
    });
}

bool MainWidget::hasChatWindow(int id)
{
    for(int i =  0; i < chatWindowVec.size(); ++i) {
        if(id == chatWindowVec[i]->socketID) {
            return true;
        }
    }
    return false;
}

void MainWidget::connected()
{
    ui->statusBox->setCurrentText("Online");

    socket->write("localHostName: " + QHostInfo::localHostName().toUtf8());
}

void MainWidget::unconnected()
{
    ui->statusBox->setCurrentText("Offline");
}

void MainWidget::statusChanged(int index)
{
    if(index == 0) {
        socket->connectToServer();
    }
    else {
        socket->close();
    }
}

void MainWidget::readMessage()
{
    QString message = socket->readAll();

    if(message.contains(" NextSocket: ")) {
        recieveSocket(message);
    }
    else if(message.contains("disconnectedSocket: ")) {
        socketDisconnected(message);
    }
    else if(message.contains("messageFrom: ")) {
        recieveMessage(message);
    }
}

void MainWidget::userListDoubleClicked(QListWidgetItem *item)
{
    item->setTextColor(QColor()); //set text color to the default color

    if(hasChatWindow(userIDVec[ui->userList->currentRow()].toInt())) {
        for(int i =  0; i < chatWindowVec.size(); ++i) {
            if(userIDVec[ui->userList->currentRow()].toInt() == chatWindowVec[i]->socketID) {
                chatWindowVec[i]->showNormal();
            }
        }
    }
    else {
        createChatWindow(item);
    }
}
