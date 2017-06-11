#ifndef PEOPLEINFO_H
#define PEOPLEINFO_H

#include "peopleinfotab1.h"
#include "peopleinfotab2.h"
#include <QTabWidget>

class PeopleInfo : public QTabWidget
{
	PeopleInfoTab1* tab1;
	PeopleInfoTab2* tab2;
	int next_tab;

	bool canTabClose();

	Q_OBJECT
public:
	explicit PeopleInfo(QWidget *parent = 0);

	~PeopleInfo();
signals:
	void savedId(int);
	void error(QString);

public slots:
	void setSqlId(int sql_id);

private slots:
	void emitError(QString);
	void clickTab(int);
	void changeTab(int);

protected:
	void closeEvent(QCloseEvent *);
};

#endif // PEOPLEINFO_H
