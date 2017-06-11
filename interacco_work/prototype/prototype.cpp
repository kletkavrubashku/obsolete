#include "prototype.h"

QString delFrom(const QString &source, const QString &search){
	return source.left(source.indexOf(search, 0, Qt::CaseInsensitive));
}

QString setLimit(const QString &source, const QString &limit_str){
	QString q_str(source);
	q_str = delFrom(q_str, " limit");
	if (limit_str != "")
		q_str += " limit " + limit_str;
	return q_str;
}

QString setOrder(const QString &source, const QString &order_str){
	QString q_str(source);
	q_str = setLimit(q_str, "");
	q_str = delFrom(q_str, " order by");
	if (order_str != "")
		q_str += " order by " + order_str;
	return q_str;
}

QString setWhere(const QString &source, const QString &where_str){
	QString q_str(source), order_str;
	q_str = setLimit(q_str, "");
	qint32 order_pos = q_str.indexOf("order by", 0, Qt::CaseInsensitive);
	if (order_pos != -1)
		order_str = q_str.right(q_str.length() - order_pos - 9);
	q_str = setOrder(q_str, "");
	q_str = delFrom(q_str, " where");	
	if (where_str != "")
		q_str += " where " + where_str;
	q_str = setOrder(q_str, order_str);
	return q_str;
}

QString logger(const QSqlQuery &q, QString pattern) {
	QString res = QString("Error - %1%2\n\tdriver:\t%3\n\tquery:\t%4");
	res = res.arg(q.lastError().databaseText());
	res = res.arg(pattern);
	res = res.arg(q.lastError().driverText());
	res = res.arg(q.lastQuery());
	qDebug() << res;
	return res;
}

QString logger(const QString &str, QString pattern) {
	QString res = QString("%1%2").arg(str).arg(pattern);
	qDebug() << res;
	return res;
}
