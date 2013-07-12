#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T10:34:51
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = zeranetclient
TEMPLATE = lib

DEFINES += CLIENTNETBASE_LIBRARY

SOURCES += \
    src/zclientnetbase.cpp \
    src/zclientnetbase_private.cpp

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
