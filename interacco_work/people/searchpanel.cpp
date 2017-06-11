#include "searchpanel.h"
#include <QDebug>
#include <QSqlQuery>

SearchPanel::SearchPanel(QWidget *parent) : QWidget(parent){
	input = new QLineEdit;
	button = new QPushButton("Search");

	input->setValidator(new QRegExpValidator(QRegExp("[\\w\\d .-]*")));

	connect(button, SIGNAL(clicked()), this, SLOT(emitSetSQLSearch()));
	connect(input, SIGNAL(returnPressed()), this, SLOT(emitSetSQLSearch()));

	QHBoxLayout* l = new QHBoxLayout;
	l->addWidget(input);
	l->addWidget(button);
	setLayout(l);
}

SearchPanel::~SearchPanel(){}

void SearchPanel::emitSetSQLSearch() {
	QStringList list(input->text().split(' ', QString::SkipEmptyParts));
	QStringList result;
	foreach(QString str, list) {
		QStringList str_result;
		QMapIterator<QString, SearchRule> i(rules);
		while (i.hasNext()) {
			i.next();
			switch (i.value()) {
			case String:
				str_result << QString("%1 LIKE '%%2%'").arg(i.key()).arg(str);
				break;
			case Date:
				break;
			}
		}
		result << QString("(%1)").arg(str_result.join(" or "));
	}
	emit setSQLSearch(result.join(" and "));
}

void SearchPanel::addSearch(QString col_name, SearchPanel::SearchRule r){
	rules[col_name] = r;
}

void SearchPanel::clearSearch() {
	rules.clear();
}
