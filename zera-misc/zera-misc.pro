#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------

QT       -= gui
QT       += network

include(misc.user.pri)

TARGET = zeramisc
TEMPLATE = lib

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
    rminterface_p.cpp

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
    rminterface_p.h

target.path = /usr/lib
INSTALLS += target

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/misc
INSTALLS += header_files
