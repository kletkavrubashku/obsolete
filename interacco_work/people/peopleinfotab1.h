#ifndef PEOPLEINFOTAB1_H
#define PEOPLEINFOTAB1_H

#include <QWidget>
#include <QPushButton>
#include "prototype/sqlwidget/sqlcombobox.h"
#include "prototype/sqlwidget/sqldateedit.h"
#include "prototype/sqlwidget/sqlimage.h"
#include "prototype/sqlwidget/sqllineedit.h"

class PeopleInfoTab1 : public QWidget
{
	int sql_id;
	bool sql_loaded;

	QMap<QString, SqlWidget*> widgets;
	QLineEdit* age;

	QPushButton* save_btn;
	QPushButton* close_btn;

	Q_OBJECT
public:
	explicit PeopleInfoTab1(QWidget *parent = 0);
	~PeopleInfoTab1();
	void setSqlId(int sql_id_);

	bool isChanged();
	bool saveForm();
	void clearForm();

signals:
	void savedId(int);
	void error(QString);
	void closeTab();
	void changedSqlId(int);

public slots:
	void save();

private slots:
	void changeAge();
	void checkChanges();

protected:
	void showEvent(QShowEvent*);
};

#endif // PEOPLEINFOTAB1_H
