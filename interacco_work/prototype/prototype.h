#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#define LOG QString("\n\tfile:\t%1\n\tfunction:\t%2\n\tline:\t%3").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)

QString delFrom(const QString &source, const QString &search);

QString setLimit(const QString &source, const QString &limit_str);
QString setOrder(const QString &source, const QString &order_str);
QString setWhere(const QString &source, const QString &where_str);

QString logger(const QSqlQuery &q, QString pattern);
QString logger(const QString &str, QString pattern);

#endif // PROTOTYPE_H
