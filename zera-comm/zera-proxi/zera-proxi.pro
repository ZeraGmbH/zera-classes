#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T07:52:09
#
#-------------------------------------------------

QT       += network
QT       -= gui

include(proxi.user.pri)

TARGET = zera-proxi
TEMPLATE = lib

LIBS +=  -lzeranetclient
LIBS +=  -lprotobuf

DEFINES += ZERAPROXI_LIBRARY

SOURCES += proxi.cpp \
    proxi_p.cpp \
    proxiclient_p.cpp

HEADERS += proxi.h \
    proxi_global.h \
    proxiconnection.h \
    proxiclient.h \
    proxiclient_p.h \
    proxi_p.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/proxi
INSTALLS += header_files

target.path = /usr/lib
INSTALLS += target


OTHER_FILES +=
