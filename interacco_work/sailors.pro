#-------------------------------------------------
#
# Project created by QtCreator 2015-02-07T21:43:22
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sailors
TEMPLATE = app


SOURCES += main.cpp\
    people/peoplewidget.cpp \
    people/searchpanel.cpp \
    people/peopleinfo.cpp \
	people/dynamicsqltablemodel.cpp \
	people/peopleinfotab1.cpp \
	prototype/prototype.cpp \
    prototype/sqlwidget/sqlcombobox.cpp \
    prototype/sqlwidget/sqldateedit.cpp \
    prototype/sqlwidget/sqlimage.cpp \
    prototype/sqlwidget/sqllineedit.cpp \
    prototype/sqlwidget/sqlwidget.cpp \
    people/peopleinfotab2.cpp \
    prototype/sqlwidget/sqlyearedit.cpp \
    prototype/waitcursor.cpp \
    prototype/sqlwidget/sqltablewidget.cpp

HEADERS  += \
    people/peoplewidget.h \
    people/searchpanel.h \
	people/peopleinfo.h \
	people/dynamicsqltablemodel.h \
    people/peopleinfotab1.h \
	prototype/prototype.h \
    prototype/sqlwidget/sqlcombobox.h \
    prototype/sqlwidget/sqldateedit.h \
    prototype/sqlwidget/sqlimage.h \
    prototype/sqlwidget/sqllineedit.h \
    prototype/sqlwidget/sqlwidget.h \
    people/peopleinfotab2.h \
    prototype/sqlwidget/sqlyearedit.h \
    prototype/waitcursor.h \
    prototype/sqlwidget/sqltablewidget.h

RESOURCES += application.qrc
