#ifndef SQLYEAREDIT_H
#define SQLYEAREDIT_H

#include <QLineEdit>
#include "sqlwidget.h"

const int null_year = 1000;

class SqlYearEdit : public QLineEdit, public SqlWidget {

	Q_OBJECT
	void tryConvertDate();

	void setYear(int y);
public:
	explicit SqlYearEdit(bool is_required_ = true, QWidget *parent = 0);
	~SqlYearEdit();


	QWidget* widget() const;
	QVariant value();
	bool isRequireAccepted();

signals:
	void changedValue();
	void error(QString);

private slots:
	void checkRequired();

protected:
	void focusOutEvent(QFocusEvent*);
	void setValue(const QVariant& year);

};

#endif // SQLYEAREDIT_H
