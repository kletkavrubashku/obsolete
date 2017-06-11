#include "peopleinfotab1.h"
#include <QFormLayout>
#include <QBoxLayout>
#include <QSqlQuery>
#include <QGroupBox>
#include "prototype/prototype.h"
#include "prototype/waitcursor.h"
#include <QDebug>
#include <QMessageBox>

PeopleInfoTab1::PeopleInfoTab1(QWidget *parent) : QWidget(parent), sql_id(0), sql_loaded(false){
	age = new QLineEdit;

	// top labels
	QFormLayout* left_top = new QFormLayout;
	left_top->addRow("&Должность:", (widgets["position"] = new SqlComboBox)->widget());
	left_top->addRow("&Фамилия:", (widgets["surname"] = new SqlLineEdit)->widget());
	left_top->addRow("&Имя:", (widgets["name"] = new SqlLineEdit)->widget());
	left_top->addRow("&Отчество:", (widgets["patronymic"] = new SqlLineEdit)->widget());
	left_top->addRow("&Дата рождения:", (widgets["birthday"] = new SqlDateEdit)->widget());
	left_top->addRow("&Место рождения:", (widgets["birthplace"] = new SqlLineEdit)->widget());
	left_top->addRow("&Гражданство:", (widgets["citizenship"] = new SqlComboBox)->widget());
	QFormLayout* right_top = new QFormLayout;
	right_top->addRow("&Судно:", (widgets["ship"] = new SqlComboBox)->widget());
	right_top->addRow("&Surname:", (widgets["surname_en"] = new SqlLineEdit)->widget());
	right_top->addRow("&Name:", (widgets["name_en"] = new SqlLineEdit)->widget());
	right_top->addRow("&Статус:", (widgets["status"] = new SqlComboBox)->widget());
	right_top->addRow("&Возраст:", age);
	right_top->addRow("&Семейное положение:", (widgets["family"] = new SqlComboBox)->widget());
	right_top->addRow("&Группа:", (widgets["category"] = new SqlComboBox)->widget());
	QHBoxLayout* top = new QHBoxLayout;
	top->addLayout(left_top);
	top->addSpacing(10);
	top->addLayout(right_top);

	// contacts layout
	QGroupBox *contacts = new QGroupBox("Контакты");
	QFormLayout* contacts_left = new QFormLayout;
	contacts_left->addRow("&Адрес:", (widgets["address"] = new SqlLineEdit)->widget());
	contacts_left->addRow("&Дом. телефон:", (widgets["home_phone"] = new SqlLineEdit)->widget());
	contacts_left->addRow("&Моб. телефон:", (widgets["mobile_phone"] = new SqlLineEdit)->widget());
	contacts_left->addRow("&Email:", (widgets["email"] = new SqlLineEdit)->widget());
	QFormLayout* contacts_right = new QFormLayout;
	contacts_right->addRow("&ФИО родственника:", (widgets["relation"] = new SqlLineEdit)->widget());
	contacts_right->addRow("&Степень родства:", (widgets["relation_degree"] = new SqlComboBox)->widget());
	contacts_right->addRow("&Телефон родтв.:", (widgets["relation_phone"] = new SqlLineEdit)->widget());
	contacts_right->addRow("&Адрес родств.:", (widgets["relation_address"] = new SqlLineEdit)->widget());
	QHBoxLayout* contacts_layout = new QHBoxLayout;
	contacts_layout->addLayout(contacts_left);
	contacts_layout->addSpacing(10);
	contacts_layout->addLayout(contacts_right);
	contacts->setLayout(contacts_layout);

	// passport layout
	QGroupBox* passport_data = new QGroupBox("Паспортные данные");
	QGridLayout* passport_layout = new QGridLayout;
	passport_layout->addWidget(new QLabel("Гражданский паспорт:"), 1,0);
	passport_layout->addWidget(new QLabel("Заграничный паспорт:"), 2,0);
	passport_layout->addWidget(new QLabel("Паспорт Моряка:"), 3,0);
	passport_layout->addWidget(new QLabel("Номер:"), 0,1);
	passport_layout->addWidget(new QLabel("Выдан:"), 0,2);
	passport_layout->addWidget(new QLabel("Дата окончания:"), 0,3);
	passport_layout->addWidget((widgets["passport_number"] = new SqlLineEdit)->widget(), 1,1);
	passport_layout->addWidget((widgets["passport_organization"] = new SqlLineEdit)->widget(), 1,2);
	passport_layout->addWidget((widgets["international_passport_number"] = new SqlLineEdit)->widget(), 2,1);
	passport_layout->addWidget((widgets["international_passport_organization"] = new SqlLineEdit)->widget(), 2,2);
	passport_layout->addWidget((widgets["international_passport_date"] = new SqlDateEdit)->widget(), 2,3);
	passport_layout->addWidget((widgets["sailor_passport_number"] = new SqlLineEdit)->widget(), 3,1);
	passport_layout->addWidget((widgets["sailor_passport_organization"] = new SqlLineEdit)->widget(), 3,2);
	passport_layout->addWidget((widgets["sailor_passport_date"] = new SqlDateEdit)->widget(), 3,3);
	passport_data->setLayout(passport_layout);

	// layout
	QVBoxLayout* form = new QVBoxLayout;
	form->addLayout(top);
	form->addStretch(1);
	form->addWidget(contacts);
	form->addStretch(1);
	form->addWidget(passport_data);

	QHBoxLayout* info = new QHBoxLayout;
	info->addLayout(form);
	info->addSpacing(10);
	info->addWidget((widgets["photo"] = new SqlImage)->widget());
	widgets["photo"]->initialize(QSize(330,440));

	// buttons
	save_btn = new QPushButton("&Сохранено");
	close_btn = new QPushButton("&Закрыть");
	connect(save_btn, SIGNAL(clicked()), this, SLOT(save()));
	connect(close_btn, SIGNAL(clicked()), this, SIGNAL(closeTab()));
	QHBoxLayout* buttons = new QHBoxLayout;
	buttons->addStretch(1);
	buttons->addWidget(save_btn);
	buttons->addWidget(close_btn);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(info);
	layout->addLayout(buttons);
	setLayout(layout);

	connect(widgets["birthday"]->widget(), SIGNAL(changedValue()), this, SLOT(changeAge()));
	age->setDisabled(true);

	save_btn->setEnabled(false);
	close_btn->setFocus();

	foreach(SqlWidget* w, widgets)
		connect(w->widget(), SIGNAL(changedValue()), this, SLOT(checkChanges()));
}

void PeopleInfoTab1::setSqlId(int sql_id_) {
	if (sql_id == sql_id_)
		return;
	sql_loaded = false;
	sql_id = sql_id_;
}


PeopleInfoTab1::~PeopleInfoTab1(){}

void PeopleInfoTab1::showEvent(QShowEvent *){
	if (sql_loaded) return;
	WaitCursor c;
	setMinimumHeight(490);

	// load ComboBox
	widgets["position"]->initialize(QSqlQuery("select id, name from position order by name"));
	widgets["citizenship"]->initialize(QSqlQuery("select id, name from citizenship order by name"));
	widgets["ship"]->initialize(QSqlQuery("select id, name from ship order by name"));

	widgets["status"]->initialize(QStringList() << "Кандидат" << "В рейсе" << "На берегу" << "На оплате" << "Архив");
	widgets["family"]->initialize(QStringList() << "" << "Женат (замужем)" << "Холост");
	widgets["category"]->initialize(QStringList() << "White List" << "Gray List" << "Black List");
	widgets["relation_degree"]->initialize(QStringList() << "" << "Жена" << "Муж" << "Мать" << "Отец" << "Брат" << "Сестра" << "Другое");

	if (sql_id <= 0) {
		sql_loaded = true;
		return;
	}
	QSqlQuery q = QSqlQuery();
	q.prepare("select " + QStringList(widgets.keys()).join(", ") + " from sailor where id=:id");
	q.bindValue(":id", sql_id);

	q.setForwardOnly(true);
	if (!q.exec() || !q.first())  {
		emit error(logger(q, LOG));
		return;
	}

	foreach (QString key, widgets.keys())
		widgets[key]->setInitialValue(q.value(key));
	sql_loaded = true;
	checkChanges();
}

void PeopleInfoTab1::save(){
	if (saveForm())
		checkChanges();
}

void PeopleInfoTab1::changeAge() {
	QDate date = widgets["birthday"]->value().toDate();
	int years_diff = 0;
	while (date.addYears(++years_diff) <= QDate::currentDate());
	QString str_end;
	switch ((years_diff-1)%10) {
	case 1:
		str_end = "год";
		break;
	case 2:
	case 3:
	case 4:
		str_end = "года";
		break;
	default:
		str_end = "лет";
	}
	age->setText(QString::number(years_diff-1) + " " + str_end);
}

void PeopleInfoTab1::checkChanges() {
	if (!sql_loaded) return;
	if (isChanged()) {
		save_btn->setText("Сохранить");
		save_btn->setEnabled(true);
	}
	else {
		save_btn->setText("Сохранено");
		save_btn->setEnabled(false);
	}
}

bool PeopleInfoTab1::saveForm(){
	WaitCursor c;
	QString action_str;

	if (sql_id > 0) {
		QStringList sets;
		foreach (QString key, widgets.keys())
			sets << key + "=:" + key;
		action_str = "update sailor set " + sets.join(", ") + " where id=:id";
	}
	else
		action_str = "insert into sailor (" + QStringList(widgets.keys()).join(", ") + ") values (:" + QStringList(widgets.keys()).join(", :") + ")";

	QSqlQuery q = QSqlQuery();
	q.prepare(action_str);
	foreach (QString key, widgets.keys()) {
		if (!widgets[key]->isRequireAccepted() || !widgets[key]->value().isValid()) {
			QMessageBox(QMessageBox::Warning, "Ошибка", "Ошибка ввода<br>Неправильно введены данные").exec();
			widgets[key]->widget()->setFocus();
			return false;
		}
		q.bindValue(":" + key, widgets[key]->value());
	}
	q.bindValue(":id", sql_id);

	if (!q.exec())  {
		emit error(logger(q, LOG));
		return false;
	}

	emit savedId(sql_id);

	if (sql_id <= 0) {
		sql_id = q.lastInsertId().toInt();
		emit changedSqlId(sql_id);
	}

	foreach(SqlWidget* w, widgets)
		w->setValueToOld();

	return true;
}

bool PeopleInfoTab1::isChanged() {
	bool is_changed = false;
	foreach(SqlWidget* w, widgets)
		is_changed = is_changed || w->isChanged();
	return is_changed;
}

void PeopleInfoTab1::clearForm() {
	foreach(SqlWidget* w, widgets)
		w->setOldToValue();
}
