#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T07:52:09
#
#-------------------------------------------------
TEMPLATE = lib
ZDEPENDS += protonet \
    resman

include(../../zera-classes.pri)

QT       += network
QT       -= gui

TARGET = zera-proxy
DEFINES += ZERAPROXY_LIBRARY

SOURCES += proxy.cpp \
    proxy_p.cpp \
    proxyclient_p.cpp \
    proxyprotobufwrapper.cpp \
    proxynetpeer.cpp

HEADERS += proxy.h \
    proxy_global.h \
    proxyconnection.h \
    proxyclient.h \
    proxyclient_p.h \
    proxy_p.h \
    proxyprotobufwrapper.h \
    proxynetpeer.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/proxy
INSTALLS += header_files
