#ifndef DYNAMICSQLTABLEMODEL_H
#define DYNAMICSQLTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>

class DynamicSqlTableModel : public QAbstractTableModel
{
	Q_OBJECT

	QString query;
	QString table_name;
	int row_count;
	int col_count;

	mutable QList<QSqlRecord>list;
	QList<QString>header;

	void initializePackageByRow(int row) const;
	void initializeRowCount();
	void initializeHeader();

	const static int package_size = 10;

public:
	explicit DynamicSqlTableModel(QObject *parent = 0);
	~DynamicSqlTableModel();

	void select(QString query_);

	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex &index = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

signals:
	void error(QString) const;

public slots:
	void refreshData();
	void refreshRows();
	void refreshById(int);
	void setSearch(QString);
};

#endif // DYNAMICSQLTABLEMODEL_H
