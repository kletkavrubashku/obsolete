#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T15:36:19
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = XOServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    minimaxnode.cpp \
    minimaxtree.cpp \
    game.cpp \
    server.cpp

HEADERS += \
    minimaxnode.h \
    minimaxtree.h \
    game.h \
    server.h
