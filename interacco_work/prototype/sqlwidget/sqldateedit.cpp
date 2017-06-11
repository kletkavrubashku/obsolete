#include "sqldateedit.h"
#include <QRegExpValidator>
#include <QMessageBox>
#include <QFocusEvent>
#include <QDebug>
#include <QGraphicsColorizeEffect>

SqlDateEdit::SqlDateEdit(bool is_required_, QWidget *parent) : QLineEdit(parent), SqlWidget(is_required_) {
	icon = new QPushButton(this);
	icon->setIcon(QIcon(":/icons/datetime.png"));
	connect(icon, SIGNAL(clicked()), this, SLOT(calendarPopup()));

	setValidator(new QRegExpValidator(QRegExp("[\\d\\.]*"), this));

	calendar = new QCalendarWidget(parent);
	calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
	connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(setDate(QDate)));
	connect(calendar, SIGNAL(activated(QDate)), this, SLOT(setDate(QDate)));
	if (is_required) {
		connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkRequired()));
		checkRequired();
	}
}

SqlDateEdit::~SqlDateEdit(){}

void SqlDateEdit::resizeEvent(QResizeEvent*) {
	icon->setFixedHeight(height());
	icon->move(width() - icon->width(),0);
	int left, top, bottom;
	getTextMargins (&left, &top, 0, &bottom);
	setTextMargins( left, top, icon->size().width(), bottom );
}

void SqlDateEdit::calendarPopup() {
	calendar->setWindowFlags(Qt::Popup);
	calendar->move(icon->mapToGlobal(icon->rect().center()));
	calendar->show();
	calendar->setFocus();
}

QVariant SqlDateEdit::value() {
	QDate d = QDate::fromString(text(), "dd.MM.yyyy");
	return text() == "" ? null_date:(d.isValid()?d:QVariant(QVariant::Invalid));
}

void SqlDateEdit::setValue(const QVariant& data) {
	setDate(data.toDate());
}

void SqlDateEdit::checkRequired() {
	if (!isRequireAccepted()) {
        QGraphicsColorizeEffect* c = new QGraphicsColorizeEffect(this);
        c->setColor(Qt::red);
        setGraphicsEffect(c);
    }
    else
        setGraphicsEffect(NULL);
}

void SqlDateEdit::setDate(QDate date) {
	QString  next_text, prev_text = text();
	if (date == null_date)
		next_text = "";
	else
		next_text = date.toString("dd.MM.yyyy");
	if (calendar->isVisible()) calendar->close();
	if (next_text != prev_text) {
		setText(next_text);
		emit changedValue();
	}
}

QWidget* SqlDateEdit::widget() const {
	return const_cast<SqlDateEdit*>(this);
}

void SqlDateEdit::focusOutEvent(QFocusEvent* e){
	QLineEdit::focusOutEvent(e);
	if (e->reason() != Qt::ActiveWindowFocusReason && !tryConvertDate()) {
		QMessageBox(QMessageBox::Warning, "Ошибка", "Ошибка ввода даты<br>Неправильно введены данные").exec();
		setFocus();
	}
	if (text() == "")
		emit changedValue();
}

bool SqlDateEdit::tryConvertDate() {
	if(text() == "") return true;
	if (QRegExp("\\d{2}\\.\\d{2}\\.\\d{4}").exactMatch(text()) && QDate::fromString(text(), "dd.MM.yyyy").isValid()) return true;
	if (QRegExp("\\d{1,2}\\.\\d{1,2}\\.\\d{1,3}").exactMatch(text())) {
		QStringList list = text().split(".");
		int day = list[0].toInt(), month = list[1].toInt(), year = list[2].toInt();
		int year_diff = QDate::currentDate().year() - year;
		switch(list[2].size()) {
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
		}
		QDate d(year, month, day);
		if (d.isValid()) {
			setDate(d);
			return true;
		}
	}
	return false;
}

bool SqlDateEdit::isRequireAccepted(){
	return !is_required || value() != null_date;
}
