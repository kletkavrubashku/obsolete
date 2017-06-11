#include "sqlwidget.h"
#include <QDebug>

SqlWidget::SqlWidget(bool is_required_) : is_required(is_required_) {}

void SqlWidget::setInitialValue(const QVariant &data) {
	old = data;
	setValue(data);
}

void SqlWidget::setOldToValue() {
	setValue(old);
}

void SqlWidget::setValueToOld() {
	old = value();
}

bool SqlWidget::isChanged() {
	return old != value();
}
void SqlWidget::initialize(QSqlQuery) {}

void SqlWidget::initialize(QStringList) {}

void SqlWidget::initialize(QSize) {}

void SqlWidget::initialize(QList<QPair<QString, QVariant> >){}

QList<QPair<QString, QVariant> > SqlWidget::initList(){
	return QList<QPair<QString, QVariant> >();
}

SqlWidget::~SqlWidget(){}
