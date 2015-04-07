#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------
TEMPLATE = lib
ZDEPENDS += resman

include(../zera-classes.pri)

QT       -= gui
QT       += network

TARGET = zeramisc

LIBS +=  -lzera-proxy

INCLUDEPATH += ../zera-comm/zera-proxy

CONFIG	+= debug
QMAKE_CXXFLAGS += -O0

DEFINES += ZERAMISC_LIBRARY

SOURCES += \
    intelhexfileio.cpp \
    intelhexfileio_p.cpp \
    ethadress.cpp \
    ethadress_p.cpp \
    pcbinterface.cpp \
    pcbinterface_p.cpp \
    interface_p.cpp \
    dspinterface.cpp \
    dspinterface_p.cpp \
    dspmeasdata.cpp \
    rminterface.cpp \
    rminterface_p.cpp \
    secinterface.cpp \
    secinterface_p.cpp

HEADERS +=\
    intelhexfileio.h \
    intelhexfileio_p.h \
    ethadress.h \
    ethadress_p.h \
    zera_misc_global.h \
    pcbinterface.h \
    pcbinterface_p.h \
    interface_p.h \
    dspinterface.h \
    dspinterface_p.h \
    dspmeasdata.h \
    dspvar.h \
    rminterface.h \
    rminterface_p.h \
    secinterface.h \
    secinterface_p.h

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/misc
INSTALLS += header_files
