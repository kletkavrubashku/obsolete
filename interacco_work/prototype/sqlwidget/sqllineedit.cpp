#include "sqllineedit.h"
#include <QGraphicsColorizeEffect>

SqlLineEdit::SqlLineEdit(bool is_required_, QWidget *parent) : QLineEdit(parent), SqlWidget(is_required_) {
	connect(this, SIGNAL(textChanged(QString)), this, SIGNAL(changedValue()));
	if (is_required) {
		connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkRequired()));
		checkRequired();
	}
}

SqlLineEdit::~SqlLineEdit(){}

void SqlLineEdit::setValue(const QVariant& data) {
	setText(data.toString());
}

QWidget* SqlLineEdit::widget() const {
	return const_cast<SqlLineEdit*>(this);
}

QVariant SqlLineEdit::value() {
	return text();
}

void SqlLineEdit::checkRequired() {
	if (!isRequireAccepted()) {
        QGraphicsColorizeEffect* c = new QGraphicsColorizeEffect(this);
        c->setColor(Qt::red);
        setGraphicsEffect(c);
    }
    else
        setGraphicsEffect(NULL);
}

bool SqlLineEdit::isRequireAccepted(){
	return !is_required || value() != "";
}
