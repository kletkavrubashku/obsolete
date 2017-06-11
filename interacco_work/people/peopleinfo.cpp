#include "peopleinfo.h"
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QEvent>
#include "prototype/prototype.h"

PeopleInfo::PeopleInfo(QWidget *parent) : QTabWidget(parent){
	setAttribute(Qt::WA_DeleteOnClose);

	tab1 = new PeopleInfoTab1(this);
	tab2 = new PeopleInfoTab2(this);
	addTab(tab1, "&Персональные данные");
	addTab(tab2, "&Образование и Документы");

	connect(this, SIGNAL(tabBarClicked(int)), this, SLOT(clickTab(int)));
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
	connect(tab1, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
	connect(tab2, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
	connect(tab1, SIGNAL(savedId(int)), this, SIGNAL(savedId(int)));
	connect(tab2, SIGNAL(savedId(int)), this, SIGNAL(savedId(int)));
	connect(tab1, SIGNAL(closeTab()), this, SLOT(close()));
	connect(tab2, SIGNAL(closeTab()), this, SLOT(close()));
	connect(tab1, SIGNAL(changedSqlId(int)), this, SLOT(setSqlId(int)));
	connect(tab2, SIGNAL(changedSqlId(int)), this, SLOT(setSqlId(int)));
}

void PeopleInfo::setSqlId(int sql_id) {
	tab1->setSqlId(sql_id);
	tab2->setSqlId(sql_id);
}

void PeopleInfo::emitError(QString str) {
	emit error(str);
}

bool PeopleInfo::canTabClose() {
	bool need_block = false;
	if (currentIndex() == 0) need_block = tab1->isChanged();
	if (currentIndex() == 1) need_block = tab2->isChanged();
	if (need_block) {
		QMessageBox ask(QMessageBox::Question, "Сохранение", "Вы хотите сохранить введённые данные?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		switch (ask.exec()) {
		case QMessageBox::Save:
			if (currentIndex() == 0) return tab1->saveForm();
			if (currentIndex() == 1) return tab2->saveForm();
		case QMessageBox::Discard:
			if (currentIndex() == 0) tab1->clearForm();
			if (currentIndex() == 1) tab2->clearForm();
			return true;
		case QMessageBox::Cancel:
			return false;
		}
	}
	return true;
}

void PeopleInfo::clickTab(int index) {
	next_tab = index != currentIndex() && canTabClose() ? index : currentIndex();
}

void PeopleInfo::changeTab(int) {
	setCurrentIndex(next_tab);
}

PeopleInfo::~PeopleInfo(){}

void PeopleInfo::closeEvent(QCloseEvent *e) {
	if (!canTabClose())
		e->ignore();
}
