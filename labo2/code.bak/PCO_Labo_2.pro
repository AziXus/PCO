#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T16:05:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


TARGET = PCO_Labo_2
TEMPLATE = app

LIBS += -lpcosynchro

SOURCES += main.cpp\
        counterincrementer.cpp \
        dialog.cpp \
        criticalsection.cpp

HEADERS  += dialog.h \
         counterincrementer.h \
         criticalsection.h

FORMS    += dialog.ui
