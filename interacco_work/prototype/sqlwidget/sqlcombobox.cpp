#include "sqlcombobox.h"
#include "../prototype.h"
#include <QGraphicsColorizeEffect>

SqlComboBox::SqlComboBox(bool is_required_, QWidget* parent): QComboBox(parent), SqlWidget(is_required_) {
	connect(this, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changedValue()));
	if (is_required)
		connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(checkRequired()));
}

SqlComboBox::~SqlComboBox(){}

void SqlComboBox::initialize(QSqlQuery q) {
	clear();
	q.setForwardOnly(true);
	if (!q.exec())  {
		emit error(logger(q, LOG));
		return;
	}
	while(q.next())
		addItem(q.value(1).toString(), q.value(0));
}

void SqlComboBox::initialize(QStringList l) {
	clear();
	foreach (QString str, l)
		addItem(str, str);
}

void SqlComboBox::initialize(QList<QPair<QString, QVariant> > l) {
	clear();
	for (int i = 0; i < l.count(); i++)
		addItem(l[i].first, l[i].second);
}

QList<QPair<QString, QVariant> > SqlComboBox::initList() {
	QList<QPair<QString, QVariant> > l;
	for (int i = 0; i < count(); i++)
		l.append(QPair<QString, QVariant>(itemText(i), itemData(i)));
	return l;
}

QWidget* SqlComboBox::widget() const {
	return const_cast<SqlComboBox*>(this);
}

QVariant SqlComboBox::value() {
	return currentData();
}

void SqlComboBox::checkRequired() {
	if (!isRequireAccepted()) {
        QGraphicsColorizeEffect* c = new QGraphicsColorizeEffect(this);
        c->setColor(Qt::red);
        setGraphicsEffect(c);
    }
    else
        setGraphicsEffect(NULL);
}

void SqlComboBox::setValue(const QVariant& data) {
	setCurrentIndex(findData(data));
}

bool SqlComboBox::isRequireAccepted(){
	return !is_required || (value() != 0 && value() != "");
}
