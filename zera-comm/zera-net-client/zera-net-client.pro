#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T10:34:51
#
#-------------------------------------------------

QT       += network

TARGET = zeranetclient
TEMPLATE = lib
VERSION = 0.0.1

DEFINES += CLIENTNETBASE_LIBRARY

SOURCES += \
    src/zeraclientnetbaseprivate.cpp \
    src/zeraclientnetbase.cpp

HEADERS +=\
    src/zeraclientnetbase.h \
    src/zeraclientnetbaseprivate.h \
    src/clientnetbase_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

LIBS += -lprotobuf

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/net-client
INSTALLS += header_files
