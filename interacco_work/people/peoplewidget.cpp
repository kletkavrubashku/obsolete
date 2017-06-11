#include "peoplewidget.h"
#include "prototype/prototype.h"
#include <QDebug>

PeopleWidget::PeopleWidget(QWidget *parent) : QWidget(parent)
{
	// search
	search = new SearchPanel;
	search->addSearch("name", SearchPanel::String);
	search->addSearch("surname", SearchPanel::String);

	// table
	table = new QTableView;
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->horizontalHeader()->setHighlightSections(false);
	model = new DynamicSqlTableModel(this);
	table->setSortingEnabled(true);
	table->sortByColumn(0, Qt::AscendingOrder);
	table->setModel(model);
	connect(search, SIGNAL(setSQLSearch(QString)), model, SLOT(setSearch(QString)));

	// connection
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("host");
	db.setDatabaseName("dbname");
	db.setUserName("username");
	db.setPassword("password");
	if (!db.open()) {
		qDebug() << "Что-то не так с соединением!";
	}
	model->select(
				setOrder(
					"select "
					"sailor.id, "
					"concat(if(surname!='', concat(surname, ' '), ''), if(sailor.name!='', concat(sailor.name, ' '), ''), patronymic) as 'ФИО', "
					"status as 'Статус', "
					"category as 'Группа', "
					"birthday as 'Дата рождения', "
					"position.name as 'Должность', "
					"ship.name as 'Судно', "
					"NULL as 'Дата списания', "
					"citizenship.name as 'Гражданство', "
					"if(mobile_phone!='',mobile_phone,home_phone) as 'Телефон', "
					"email as 'Email' "
					"from sailor "
					"left join ship on sailor.ship=ship.id "
					"left join position on sailor.position=position.id "
					"left join citizenship on sailor.citizenship=citizenship.id ",
					"sailor.id asc")
				);
	table->setColumnHidden(0,true);

	// layout
	QVBoxLayout* l = new QVBoxLayout;
	l->addWidget(search);
	l->addWidget(table);
	setLayout(l);

	add = new QPushButton(QIcon(":/icons/add.png"), "", this);
	add->setIconSize(QSize(25,25));
	add->setFixedSize(35,35);
	connect(add, SIGNAL(clicked()), this, SLOT(addSailor()));

	// peopleinfo
	connect(table, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showInfo(QModelIndex)));
}

void PeopleWidget::showInfo(QModelIndex i) {
	PeopleInfo* p_i = new PeopleInfo;
	p_i->setSqlId(model->index(i.row(),0).data().toInt());
	p_i->show();
	connect(p_i, SIGNAL(savedId(int)), model, SLOT(refreshById(int)));
}

void PeopleWidget::addSailor() {
	PeopleInfo* p_i = new PeopleInfo;
	p_i->show();
	//connect(p_i, SIGNAL(savedId(int)), model, SLOT(refreshById(int)));   //индикатор
}

PeopleWidget::~PeopleWidget(){}

void PeopleWidget::resizeEvent(QResizeEvent*){
	add->move(0, height()-add->height());
}

