#-------------------------------------------------
#
# Project created by QtCreator 2014-11-20T18:37:00
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = whatsapp-tools
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG += static

LIBS += -Llibs/ -lcurl  -lrtmp -lssh2 -lssl -lcrypto -lidn 

win32 {
	LIBS += -lwldap32 
}

QMAKE_CXXFLAGS += -DCURL_STATICLIB -Iinclude

