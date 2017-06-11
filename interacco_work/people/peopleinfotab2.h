#ifndef PEOPLEINFOTAB2_H
#define PEOPLEINFOTAB2_H

#include <QWidget>
#include <QPushButton>
#include "prototype/sqlwidget/sqlcombobox.h"
#include "prototype/sqlwidget/sqllineedit.h"
#include "prototype/sqlwidget/sqlyearedit.h"

class PeopleInfoTab2 : public QWidget
{
	int sql_id;
	bool sql_loaded;

	QMap<QString, SqlWidget*> widgets;

	QPushButton* save_btn;
	QPushButton* close_btn;

	Q_OBJECT
public:
	explicit PeopleInfoTab2(QWidget *parent = 0);
	void setSqlId(int sql_id_);

	~PeopleInfoTab2();

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
	void checkChanges();

protected:
	void showEvent(QShowEvent*);
};

#endif // PEOPLEINFOTAB2_H
