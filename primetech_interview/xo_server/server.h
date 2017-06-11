#ifndef SERVER_H
#define SERVER_H

#include "game.h"
#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>


class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();        

private slots:
    void onNewConnection();
    void readData();
    void onSocketDisconnected();

private:
    QMap<QTcpSocket*, Game*>    clients; // пользователи
    MiniMaxTree*                game_tree; // дерево игры на всех клиентов (выделяем память)
    QTcpServer*                 server;

    void processMessage(QString message);
    void sendReply(GameReply reply);

    void printReport(QString str); // вывод в консоль

};

#endif // SERVER_H
