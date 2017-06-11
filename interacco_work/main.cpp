#include "people/peoplewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	PeopleWidget w;
	w.show();
	w.setGeometry(100,100,1200,600);

	return a.exec();
}
