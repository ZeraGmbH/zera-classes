#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T07:52:09
#
#-------------------------------------------------

QT       += network
QT       -= gui

include(proxy.user.pri)

TARGET = zera-proxy
TEMPLATE = lib

LIBS +=  -lzeranetclient
LIBS +=  -lprotobuf

DEFINES += ZERAPROXY_LIBRARY

SOURCES += proxy.cpp \
    proxy_p.cpp \
    proxyclient_p.cpp

HEADERS += proxy.h \
    proxy_global.h \
    proxyconnection.h \
    proxyclient.h \
    proxyclient_p.h \
    proxy_p.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/proxy
INSTALLS += header_files

target.path = /usr/lib
INSTALLS += target


OTHER_FILES +=
