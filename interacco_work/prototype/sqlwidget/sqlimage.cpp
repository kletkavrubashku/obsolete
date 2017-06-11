#include "sqlimage.h"
#include <QPainter>
#include <QFileDialog>
#include <QBuffer>
#include <QAction>
#include <QMouseEvent>
#include "../prototype.h"
#include <QGraphicsColorizeEffect>

SqlImage::SqlImage(bool is_required_, QWidget *parent) : QLabel(parent), SqlWidget(is_required_) {
	QAction* clear = new QAction(QIcon(":/icons/delete.png"), "&Удалить изображение", this);
	addAction(clear);
	setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(clear, SIGNAL(triggered()), this, SLOT(setDefault()));

	setCursor(Qt::PointingHandCursor);
	setAlignment(Qt::AlignCenter);
}

SqlImage::~SqlImage(){}

QWidget* SqlImage::widget() const {
	return const_cast<SqlImage*>(this);
}

void SqlImage::initialize(QSize size_) {
	size = size_;
	setDefault();
}

void SqlImage::setValue(const QVariant& data) {
	QByteArray byte_array = data.toByteArray();
	if (!byte_array.isEmpty() && image.loadFromData(byte_array)) {
		is_empty = false;
		emit changedValue();
		if (is_required)
			checkRequired();
		update();
	} else
		setDefault();
}

QVariant SqlImage::value() {
	if (is_empty) return QString("");
	QByteArray b_a;
	QBuffer buffer(&b_a);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "JPG");
	return b_a;
}

void SqlImage::update() {
	setFixedSize(image.width(),image.height());
	leaveEvent(NULL);
}

void SqlImage::enterEvent(QEvent*){
	QPixmap p(image);
	QPainter painter(&p);
	QPen pen(Qt::black, 2);
	painter.setPen(pen);
	painter.setBrush(QColor(0,0,0,30));
	QRect rect(p.rect() - QMargins(1, 1, 1, 1));
	painter.drawRect(rect);
	painter.drawText(rect, Qt::AlignCenter, is_empty?"Добавить фото":"Изменить фото");
	painter.end();
	setPixmap(p);
}

void SqlImage::leaveEvent(QEvent*){
	QPixmap p(image);
	QPainter painter(&p);
	QPen pen(Qt::black, 1);
	painter.setPen(QPen(pen));
	QRect rect(p.rect() - QMargins(0, 0, 1, 1));
	painter.drawRect(rect);

	painter.end();
	setPixmap(p);
}

void SqlImage::mousePressEvent(QMouseEvent* e){
	if (e->button() != Qt::LeftButton) return;
	QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Image Files (*.jpg *.png *.gif *.jpeg *.bmp)");

	if (file_name != "") {
		QFile file(file_name);
		if (!file.open(QIODevice::ReadOnly)) {
			emit error(logger(file.errorString(), LOG));
			return;
		}
		file.close();
		image = QPixmap(file_name).scaled(size.width(), size.height(), Qt::KeepAspectRatio);
		is_empty = false;
		emit changedValue();
		if (is_required)
			checkRequired();
		update();
	}
}

void SqlImage::setDefault(){
	is_empty = true;
	image = QPixmap(size);
	QPainter painter(&image);
	QLinearGradient l_g(image.rect().topLeft(), image.rect().bottomRight());
	l_g.setColorAt(0, Qt::white);
	l_g.setColorAt(0.5, QColor(180,180,180));
	l_g.setColorAt(1, Qt::white);
	painter.setPen(QPen());
	painter.fillRect(image.rect(), l_g);
	painter.end();
	if (is_required)
		checkRequired();
	emit changedValue();
	update();
}

void SqlImage::checkRequired() {
	if (!isRequireAccepted()) {
        QGraphicsColorizeEffect* c = new QGraphicsColorizeEffect(this);
        c->setColor(Qt::red);
        setGraphicsEffect(c);
    }
    else
        setGraphicsEffect(NULL);
}

bool SqlImage::isRequireAccepted(){
	return !is_required || !is_empty;
}
