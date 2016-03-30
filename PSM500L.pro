#-------------------------------------------------
#
# Project created by hunght 2016-03-23T14:14:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PSM500L
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    hled.cpp \
    function_Check.cpp

HEADERS  += dialog.h \
    hled.h \
    function_Check.h

FORMS    += dialog.ui
include(/home/hunght/Desktop/Qt/SNIP/PSM500L/src/qextserialport.pri)
