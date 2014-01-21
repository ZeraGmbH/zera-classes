#-------------------------------------------------
#
# Project created by QtCreator 2014-01-20T08:24:52
#
#-------------------------------------------------

QT       -= gui
QT       += network

include(pcbproxy.user.pri)

TARGET = zerapcbproxy
TEMPLATE = lib

DEPENDPATH += . src
INCLUDEPATH += .

LIBS +=  -lzeranetclient
LIBS +=  -lprotobuf

DEFINES += ZERAPCBPROXY_LIBRARY

SOURCES += pcbproxy.cpp \
    pcbproxy_p.cpp \
    interface_p.cpp

HEADERS += pcbproxy.h \
    pcbproxy_global.h \
    pcbproxy_p.h \
    interface.h \
    interface_p.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/pcbproxy
INSTALLS += header_files

target.path = /usr/lib
INSTALLS += target

OTHER_FILES +=

