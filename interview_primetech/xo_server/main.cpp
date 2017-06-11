#include <QCoreApplication>

#include "server.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server xo_game;

    return a.exec();
}
