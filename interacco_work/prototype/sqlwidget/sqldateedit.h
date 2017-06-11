#ifndef SQLDATEEDIT_H
#define SQLDATEEDIT_H

#include <QLineEdit>
#include <QPushButton>
#include <QCalendarWidget>
#include "sqlwidget.h"

const QDate null_date(1000,1,1);

class SqlDateEdit : public QLineEdit, public SqlWidget {
	QPushButton* icon;
	QCalendarWidget* calendar;

	bool tryConvertDate();

	Q_OBJECT
public:
	explicit SqlDateEdit(bool is_required_ = true, QWidget *parent = 0);
	~SqlDateEdit();

	QWidget* widget() const;
	QVariant value();
	bool isRequireAccepted();

protected:
	void resizeEvent(QResizeEvent*);
	void focusOutEvent(QFocusEvent*);
	void setValue(const QVariant& date);

signals:
	void changedValue();
	void error(QString);

private slots:
	void checkRequired();
	void setDate(QDate d);

private slots:
	void calendarPopup();
};

#endif // SQLDATEEDIT_H
