#include <QtGui/QGuiApplication>
#include <QDebug>
#include <QDir>
#include "captcha.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QDir::setCurrent("..");
    Captcha test(QImage("1.png"));
    test.ShadowEdges();
    qDebug() << test.PrintGraph();
    test.DrawImage().save("image.bmp");
    test.DrawGraph().save("graph.bmp");
    return a.exec();
}
