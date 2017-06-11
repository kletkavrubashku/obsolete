#include "server.h"
#include <QNetworkInterface>
#include <QDebug>
#include <QRegExp>

Server::Server(QObject *parent) :
    QObject(parent) {
    game_tree = new MiniMaxTree(); // строим дерево
    game_tree->growTree();

    server = new QTcpServer(this);
    if (server->listen()) {
        // выбираем сетевой интерфейс
        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        if (ipAddress.isEmpty())
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        //

        connect(server, SIGNAL(newConnection()),
                this, SLOT(onNewConnection()));

        printReport(tr("The server is running on IP: %1 port: %2").arg(ipAddress).arg(server->serverPort()));
    }
    else
        printReport(tr("Unable to start the server: %1").arg(server->errorString()));
}

void Server::onNewConnection() {
    QTcpSocket *client_socket = server->nextPendingConnection();
    QObject::connect(client_socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(client_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

    clients[client_socket] = new Game(game_tree);
    long ptr = reinterpret_cast<long>(client_socket); // ptr - идентификатор клиента
    printReport(tr("%1: User connected").arg(ptr));
}

void Server::processMessage(QString message) {
    QTcpSocket* client_socket = qobject_cast<QTcpSocket*>(sender());
    GameReply reply; // ответ сервера
    QRegExp start_rx("^start:([xo]):$");
    QRegExp goto_rx("^goto:([0-8]):$");
    if (start_rx.indexIn(message) != -1)
        reply = clients[client_socket]->startByXO(start_rx.cap(1) != "x");
    else if (goto_rx.indexIn(message) != -1)
        reply = clients[client_socket]->goTo(goto_rx.cap(1).toInt());
    else
        reply.is_error = true;
    sendReply(reply);
}

void Server::readData() {
    QTcpSocket* client_socket = qobject_cast<QTcpSocket*>(sender()); // кто отправил запрос
    // не будем волноваться о больших строках и нескольких пакетах
    // так как по выбранному протоколы длины ответов достаточно малы
    processMessage(client_socket->readAll().trimmed());
}

void Server::onSocketDisconnected() {
    QTcpSocket* client_socket = qobject_cast<QTcpSocket*>(sender());
    long ptr = reinterpret_cast<long>(client_socket);
    printReport(tr("%1: User disconnected").arg(ptr));
    // чистим память
    delete clients[client_socket]; // delete Game*
    client_socket->deleteLater();  // delete QTcpSocket*
    clients.remove(client_socket);
}

Server::~Server() {
    delete game_tree;
    QMap<QTcpSocket*, Game*>::iterator i = clients.begin();
    while (i != clients.end()) {
        i.key()->deleteLater(); // delete QTcpSocket*
        delete i.value(); // delete Game*
        i++;
    }
    clients.clear();
}

void Server::sendReply(GameReply reply) {
    QTcpSocket* client_socket = qobject_cast<QTcpSocket*>(sender());
    QString str = QString("%1:%2:%3:%4:\n").arg(reply.is_error).arg(reply.is_end_of_game).arg(reply.who_win).arg(reply.last_move);
    client_socket->write(str.toUtf8());

    // если конец игры
    if (reply.is_end_of_game) {
        long ptr = reinterpret_cast<long>(client_socket);
        QString who_wins = (reply.who_win == clients[client_socket]->getServerPlaysBy())?"Server":"User";
        switch(reply.who_win) {
        case XO::X_WIN:
            printReport(tr("%1: %2 wins game by X").arg(ptr).arg(who_wins));
            break;
        case XO::O_WIN:
            printReport(tr("%1: %2 wins game by O").arg(ptr).arg(who_wins));
            break;
        case XO::NO_WIN:
            printReport(tr("%1: Draw game").arg(ptr));
            break;
        }
    }
}

void Server::printReport(QString str) {
    qDebug() << qPrintable(str);
}
