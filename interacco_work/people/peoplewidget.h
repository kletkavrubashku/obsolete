#ifndef PEOPLEWIDGET_H
#define PEOPLEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QLayout>
#include "dynamicsqltablemodel.h"
#include "searchpanel.h"
#include "peopleinfo.h"
#include <QHeaderView>

class PeopleWidget : public QWidget
{
	Q_OBJECT

	SearchPanel* search;

	QTableView* table;
	DynamicSqlTableModel* model;

	QPushButton *add;

public:
	explicit PeopleWidget(QWidget *parent = 0);
	~PeopleWidget();

private slots:
	void showInfo(QModelIndex i);
	void addSailor();

protected:
	void resizeEvent(QResizeEvent*);
};

#endif // PEOPLEWIDGET_H
