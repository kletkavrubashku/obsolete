#ifndef SQLWIDGET_H
#define SQLWIDGET_H

#include <QSqlQuery>
#include <QSize>
#include <QVariant>

class SqlWidget {
	QVariant old;

public:
	SqlWidget(bool is_required_);
	virtual ~SqlWidget();

	void setInitialValue(const QVariant& data);
	void setOldToValue();
	void setValueToOld();
	bool isChanged();

	virtual void initialize(QSqlQuery);
	virtual void initialize(QStringList);
	virtual void initialize(QSize);
	virtual void initialize(QList<QPair<QString,QVariant> >);
	virtual QList<QPair<QString,QVariant> > initList();

	virtual QWidget* widget() const = 0;
	virtual QVariant value() = 0;
	virtual bool isRequireAccepted() = 0;

protected:
	virtual void setValue(const QVariant& data) = 0;
	bool is_required;

};

#endif // SQLWIDGET_H
