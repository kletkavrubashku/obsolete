#include "sqlyearedit.h"
#include <QFocusEvent>
#include <QRegExpValidator>
#include <QDate>
#include <QDebug>
#include <QGraphicsColorizeEffect>

SqlYearEdit::SqlYearEdit(bool is_required_, QWidget *parent) : QLineEdit(parent), SqlWidget(is_required_) {
	setValidator(new QRegExpValidator(QRegExp("\\d\\d\\d\\d"), this));
	if (is_required) {
		connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkRequired()));
		checkRequired();
	}
}

void SqlYearEdit::setYear(int y) {
	QString prev_text = text();
	QString y_str = QString::number(y);
	while(y_str.count() < 4) y_str = "0" + y_str;
	if (prev_text != y_str) {
		setText(y_str);
		emit changedValue();
	}
}

SqlYearEdit::~SqlYearEdit(){}

QWidget *SqlYearEdit::widget() const {
	return const_cast<SqlYearEdit*>(this);
}

QVariant SqlYearEdit::value() {
	return text() == "" ? null_year : text().toInt();
}

void SqlYearEdit::focusOutEvent(QFocusEvent *e) {
	QLineEdit::focusOutEvent(e);
	if (e->reason() != Qt::ActiveWindowFocusReason)
		tryConvertDate();
	if (text() == "")
		emit changedValue();
}

void SqlYearEdit::setValue(const QVariant &year) {
	if (year == null_year)
		setText("");
	else
		setYear(year.toInt());
}

void SqlYearEdit::tryConvertDate() {
	if (text() == "") return;
	int year = text().toInt();
	if (text().length() == 4) return;
	int year_diff = QDate::currentDate().year() - year;
	switch (text().count()) {
	case 3:
		if (year_diff < 2000)
			year += 1000;
		else
			year += 2000;
		break;
	case 2:
		if (year_diff < 2000)
			year += 1900;
		else
			year += 2000;
		break;
	case 1:
		year += 2000;
		break;
	case 0:
		return;
	}
	setYear(year);
}

void SqlYearEdit::checkRequired() {
	if (!isRequireAccepted()) {
        QGraphicsColorizeEffect* c = new QGraphicsColorizeEffect(this);
        c->setColor(Qt::red);
        setGraphicsEffect(c);
    }
    else
        setGraphicsEffect(NULL);
}

bool SqlYearEdit::isRequireAccepted(){
	return !is_required || value() != null_year;
}
