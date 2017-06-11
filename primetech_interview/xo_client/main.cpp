#include "gamedialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameDialog w;
    w.show();

    return a.exec();
}
