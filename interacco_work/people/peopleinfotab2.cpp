#include "peopleinfotab2.h"
#include <QFormLayout>
#include <QBoxLayout>
#include <QSqlQuery>
#include <QGroupBox>
#include "prototype/prototype.h"
#include "prototype/waitcursor.h"
#include <QLabel>
#include <QDebug>
#include <QMessageBox>

PeopleInfoTab2::PeopleInfoTab2(QWidget *parent) : QWidget(parent), sql_id(0), sql_loaded(false){
	QGroupBox* education = new QGroupBox("Образование");
	QFormLayout* education_layout = new QFormLayout;
	education_layout->addRow("&Образование:", (widgets["education_type"] = new SqlComboBox)->widget());
	education_layout->addRow("&Учебное заведение:", (widgets["education_organization"] = new SqlLineEdit)->widget());
	education_layout->addRow("&Начало обучения:", (widgets["education_from"] = new SqlYearEdit)->widget());
	education_layout->addRow("&Окончание обучения:", (widgets["education_to"] = new SqlYearEdit)->widget());
	education_layout->addRow("&Специальность:", (widgets["education_speciality"] = new SqlLineEdit)->widget());
	education->setLayout(education_layout);

	QGroupBox* additional_education = new QGroupBox("Дополнительное образование");
	QFormLayout* additional_education_layout = new QFormLayout;
	additional_education_layout->addRow("&Образование:", (widgets["additional_education_type"] = new SqlComboBox)->widget());
	additional_education_layout->addRow("&Учебное заведение:", (widgets["additional_education_organization"] = new SqlLineEdit)->widget());
	additional_education_layout->addRow("&Окончание обучения:", (widgets["additional_education_to"] = new SqlYearEdit)->widget());
	additional_education_layout->addRow("&Специальность:", (widgets["additional_education_speciality"] = new SqlLineEdit)->widget());
	additional_education->setLayout(additional_education_layout);

	QGroupBox* languages = new QGroupBox("Владение языками");
	QGridLayout* languages_layout = new QGridLayout;
	languages_layout->addWidget(new QLabel("Язык:"), 0, 0);
	languages_layout->addWidget(new QLabel("Уровень:"), 0, 1);
	languages_layout->addWidget((widgets["language1"] = new SqlComboBox)->widget(), 1, 0);
	languages_layout->addWidget((widgets["language1_level"] = new SqlComboBox)->widget(), 1, 1);
	languages_layout->addWidget((widgets["language2"] = new SqlComboBox)->widget(), 2, 0);
	languages_layout->addWidget((widgets["language2_level"] = new SqlComboBox)->widget(), 2, 1);
	languages_layout->addWidget((widgets["language3"] = new SqlComboBox)->widget(), 3, 0);
	languages_layout->addWidget((widgets["language3_level"] = new SqlComboBox)->widget(), 3, 1);
	languages->setLayout(languages_layout);

	QVBoxLayout* left = new QVBoxLayout;
	left->addWidget(education);
	left->addWidget(additional_education);
	QVBoxLayout* right = new QVBoxLayout;
	right->addWidget(languages);
	right->addStretch(1);

	QHBoxLayout* info = new QHBoxLayout;
	info->addLayout(left, 1);
	info->addLayout(right, 1);

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

	save_btn->setEnabled(false);
	close_btn->setFocus();

	foreach(SqlWidget* w, widgets)
		connect(w->widget(), SIGNAL(changedValue()), this, SLOT(checkChanges()));
}

void PeopleInfoTab2::setSqlId(int sql_id_) {
	if (sql_id == sql_id_)
		return;
	sql_loaded = false;
	sql_id = sql_id_;
}

PeopleInfoTab2::~PeopleInfoTab2(){}

void PeopleInfoTab2::save(){
	if (saveForm())
		checkChanges();
}

void PeopleInfoTab2::showEvent(QShowEvent *) {
	if (sql_loaded) return;
	WaitCursor c;

	// load ComboBox
	widgets["language1"]->initialize(QSqlQuery("select id, name from language order by name"));
	widgets["language2"]->initialize(widgets["language1"]->initList());
	widgets["language3"]->initialize(widgets["language1"]->initList());
	widgets["language1_level"]->initialize(QStringList()<< "" << "Удовлетворительный" << "Хороший" << "Отличный" << "Родной");
	widgets["language2_level"]->initialize(widgets["language1_level"]->initList());
	widgets["language3_level"]->initialize(widgets["language1_level"]->initList());

	widgets["education_type"]->initialize(QStringList() << "" << "Высшее" << "Неоконченное высшее" << "Средне-специальное" << "Среднее");
	widgets["additional_education_type"]->initialize(widgets["education_type"]->initList());

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

void PeopleInfoTab2::checkChanges() {
	if(!sql_loaded) return;
	if (isChanged()) {
		save_btn->setText("Сохранить");
		save_btn->setEnabled(true);
	}
	else {
		save_btn->setText("Сохранено");
		save_btn->setEnabled(false);
	}
}

bool PeopleInfoTab2::saveForm(){
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

bool PeopleInfoTab2::isChanged() {
	bool is_changed = false;
	foreach(SqlWidget* w, widgets)
		is_changed = is_changed || w->isChanged();
	return is_changed;
}

void PeopleInfoTab2::clearForm() {
	foreach(SqlWidget* w, widgets)
		w->setOldToValue();
}


