#ifndef SQLIMAGE_H
#define SQLIMAGE_H

#include <QLabel>
#include "sqlwidget.h"

class SqlImage : public QLabel, public SqlWidget {
	QPixmap image;
	QSize size;
	bool is_empty;

	void update();

	Q_OBJECT
public:
	explicit SqlImage(bool is_required_ = true, QWidget *parent = 0);
	~SqlImage();

	QWidget* widget() const;
	QVariant value();
	void initialize(QSize size);
	bool isRequireAccepted();

private slots:
	void setDefault();

signals:
	void changedValue();
	void error(QString);

private slots:
	void checkRequired();

protected:
	void setValue(const QVariant& data);
	void enterEvent(QEvent*);
	void leaveEvent(QEvent*);
	void mousePressEvent(QMouseEvent*);

};

#endif // QQLIMAGE_H
