#ifndef SQLLINEEDIT_H
#define SQLLINEEDIT_H

#include <QLineEdit>
#include "sqlwidget.h"

class SqlLineEdit : public QLineEdit, public SqlWidget
{
public:
	explicit SqlLineEdit(bool is_required_ = true, QWidget *parent = 0);
	~SqlLineEdit();

	QWidget* widget() const;
	QVariant value();
	bool isRequireAccepted();

	Q_OBJECT
signals:
	void changedValue();
	void error(QString);

private slots:
	void checkRequired();

protected:
	void setValue(const QVariant& data);

};

#endif // SQLLINEEDIT_H
