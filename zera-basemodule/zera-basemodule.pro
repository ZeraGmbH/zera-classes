#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------

QT       -= gui
QT       += network

include(basemodule.user.pri)

TARGET = zerabasemodule
TEMPLATE = lib

LIBS +=  -lzera-proxy
LIBS +=  -lprotobuf
LIBS +=  -lzera-resourcemanager-protobuf

INCLUDEPATH += ../zera-comm/zera-proxy

CONFIG	+= debug
QMAKE_CXXFLAGS += -O0

DEFINES += ZERABASEMODULE_LIBRARY

target.path = /usr/lib
INSTALLS += target

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/basemodule
INSTALLS += header_files

HEADERS += \
    basemeasprogram.h \
    moduleactivist.h \
    errormessages.h \
    moduleerror.h \
    moduleparameter.h \
    measchannelinfo.h \
    xmlsettings.h \
    socket.h \
    reply.h \
    modulesignal.h \
    basemodule.h \
    moduleinfo.h \
    basemoduleconfiguration.h \
    abstractmodulefactory.h \
    virtualmodule.h \
    measmodeinfo.h \
    foutinfo.h \
    rangeinfo.h \
    basemeaschannel.h \
    movingwindowfilter.h \
    basesamplechannel.h

SOURCES += \
    basemeasprogram.cpp \
    moduleerror.cpp \
    moduleparameter.cpp \
    modulesignal.cpp \
    basemoduleconfiguration.cpp \
    moduleinfo.cpp \
    moduleactivist.cpp \
    basemodule.cpp \
    measmodeinfo.cpp \
    basemeaschannel.cpp \
    movingwindowfilter.cpp \
    basesamplechannel.cpp
