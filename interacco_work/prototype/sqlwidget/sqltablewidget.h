#ifndef SQLTABLEWIDGET_H
#define SQLTABLEWIDGET_H

#include <QTableWidget>

class SqlTableWidget : public QTableWidget {

    Q_OBJECT
public:
    explicit SqlTableWidget(QWidget *parent = 0);
    ~SqlTableWidget();
};

#endif // SQLTABLEWIDGET_H
