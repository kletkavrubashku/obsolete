#ifndef SQLCOMBOBOX_H
#define SQLCOMBOBOX_H

#include <QComboBox>
#include "sqlwidget.h"

class SqlComboBox : public QComboBox, public SqlWidget {
	Q_OBJECT
public:
	explicit SqlComboBox(bool is_required_ = true, QWidget* parent = 0);
	~SqlComboBox();

	void initialize(QSqlQuery q);
	void initialize(QStringList l);
	void initialize(QList<QPair<QString, QVariant> > l);
	QList<QPair<QString, QVariant> > initList();


	QWidget* widget() const;
	QVariant value();
	bool isRequireAccepted();

signals:
	void changedValue();
	void error(QString);

private slots:
	void checkRequired();

protected:
	void setValue(const QVariant& value);
};

#endif // SQLCOMBOBOX_H
