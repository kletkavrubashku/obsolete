#include "dynamicsqltablemodel.h"
#include "prototype/sqlwidget/sqldateedit.h"
#include "prototype/prototype.h"
#include <QDate>
#include <QDebug>

DynamicSqlTableModel::DynamicSqlTableModel(QObject *parent) : QAbstractTableModel(parent), row_count(0), col_count(0) {}

DynamicSqlTableModel::~DynamicSqlTableModel(){}

QVariant DynamicSqlTableModel::data(const QModelIndex &index, int role) const {
	if (role == Qt::DisplayRole) {
		initializePackageByRow(index.row());
		QVariant data = list[index.row()].value(index.column());
		if (data.type() == QVariant::Date)
			data = data.toDate() == null_date ? "" : data.toDate().toString("dd.MM.yyyy");
		return data;
	}
	return QVariant();
}

int DynamicSqlTableModel::columnCount(const QModelIndex&) const {
	return col_count;
}
int DynamicSqlTableModel::rowCount(const QModelIndex&) const {
	return row_count;
}

QVariant DynamicSqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal)
			return header[section];
		else
			return QVariant(section);
	}
	return QVariant();
}

void DynamicSqlTableModel::initializePackageByRow(int row) const {
	for (int i = list.size(); i <= row; i+= package_size) {
		QString package_query = setLimit(query, QString("%1, %2").arg(i).arg(package_size));

		QSqlQuery q = QSqlQuery();
		q.setForwardOnly(true);
		if (!q.exec(package_query))  {
			emit error(logger(q, LOG));
			return;
		}
		while(q.next())
			list.push_back(q.record());
	}
}

void DynamicSqlTableModel::select(QString query_) {
	query = query_;
	QRegExp table_rx("from\\s*(\\S*)");
	table_rx.indexIn(query);
	table_name = table_rx.cap(1);

	initializeHeader();
	refreshRows();
}

void DynamicSqlTableModel::sort(int column, Qt::SortOrder order) {
	query = setOrder(query,QString("%1 %2").arg(column+1).arg(order?"desc":"asc"));

	refreshData();
}

void DynamicSqlTableModel::initializeRowCount() {
	QString count_query = query;
	count_query.replace(QRegExp("select(.*)from"), "select count(*) from");
	count_query = setOrder(count_query, "");

	QSqlQuery q = QSqlQuery();
	if (!q.exec(count_query))  {
		emit error(logger(q, LOG));
		return;
	}
	if (!q.first())  {
		emit error(logger(q, LOG));
		return;
	}
	row_count = q.value(0).toInt();
}

void DynamicSqlTableModel::initializeHeader() {
	QString one_str_query = setLimit(query,QString("%1, %2").arg(0).arg(0));

	QSqlQuery q = QSqlQuery();
	if (!q.exec(one_str_query))  {
		emit error(logger(q, LOG));
		return;
	}

	header.clear();
	col_count = q.record().count();
	for (int i = 0; i < col_count; i++)
		header.push_back(q.record().fieldName(i));
}

void DynamicSqlTableModel::refreshRows() {
	initializeRowCount();
	refreshData();
}

void DynamicSqlTableModel::refreshData() {
	list.clear();
	emit endResetModel(); //Linux
}

void DynamicSqlTableModel::setSearch(QString where_str) {
	query = setWhere(query, where_str);

	refreshRows();
}

void DynamicSqlTableModel::refreshById(int sql_id) {
	QString one_str_query = setWhere(query, table_name + ".id=:id");

	QSqlQuery q = QSqlQuery();
	q.prepare(one_str_query);
	q.bindValue(":id", sql_id);

	if (!q.exec())  {
		emit error(logger(q, LOG));
		return;
	}
	if (!q.first())  {
		emit error(logger(q, LOG));
		return;
	}
	for (int i = 0; i < list.size(); i++) {
		if (list[i].value("id").toInt() == sql_id) {
			list[i] = q.record();
			emit dataChanged(index(i,0),index(i,col_count-1));
			break;
		}
	}
}
