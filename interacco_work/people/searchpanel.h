#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>

class SearchPanel : public QWidget
{
	QLineEdit* input;
	QPushButton* button;

	Q_OBJECT

public:
	enum SearchRule {
		String,
		Date
	};

	explicit SearchPanel(QWidget *parent = 0);
	~SearchPanel();

	void addSearch(QString col_name, SearchPanel::SearchRule r);
	void clearSearch();

private:
	QMap<QString, SearchRule> rules;

signals:
	void setSQLSearch(QString);

private slots:
	void emitSetSQLSearch();

};

#endif // SEARCHPANEL_H
