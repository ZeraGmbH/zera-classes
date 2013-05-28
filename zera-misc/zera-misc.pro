#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------

QT       -= gui

TARGET = zera-misc
TEMPLATE = lib

DEFINES += ZERAMISC_LIBRARY

SOURCES += \
    intelhexfileio.cpp \
    intelhexfileio_p.cpp

HEADERS +=\
    zera-misc_global.h \
    intelhexfileio.h \
    intelhexfileio_p.h

target.path = /usr/lib
INSTALLS += target
