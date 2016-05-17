#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------
TEMPLATE = lib

ZDEPENDS += resman zera-validator

QT       -= gui
QT       += network

TARGET = zerabasemodule

include(../zera-modules/zera-modules.pri)
include(../zera-classes.pri)


LIBS +=  -lzera-proxy
LIBS +=  -lMeasurementModuleInterface
LIBS +=  -lzera-validator

INCLUDEPATH += ../zera-modules/interface \
    ../zera-xml-config/src \
    ../zera-misc \
    ../zera-comm/zera-proxy \
    ../zera-validator


CONFIG	+= debug
QMAKE_CXXFLAGS += -O0

DEFINES += ZERABASEMODULE_LIBRARY

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-classes/basemodule
INSTALLS += header_files

HEADERS += \
    basemeasprogram.h \
    moduleactivist.h \
    errormessages.h \
    measchannelinfo.h \
    xmlsettings.h \
    socket.h \
    reply.h \
    basemodule.h \
    basemoduleconfiguration.h \
    measmodeinfo.h \
    foutinfo.h \
    rangeinfo.h \
    basemeaschannel.h \
    movingwindowfilter.h \
    basesamplechannel.h \
    secinputinfo.h \
    basedspmeasprogram.h \
    ecalcinfo.h \
    paramvalidator.h \
    boolvalidator.h \
    intvalidator.h \
    stringvalidator.h \
    doublevalidator.h \
    basemeasworkprogram.h \
    modulevalidator.h \
    veinmodulecomponent.h \
    veinmoduleparameter.h \
    metadata.h \
    veinmodulemetadata.h \
    veinmoduleactvalue.h \
    scpiinfo.h \
    basemeasmodule.h \
    debug.h \
    basemoduleeventsystem.h \
    veinmodulecomponentinput.h

SOURCES += \
    basemeasprogram.cpp \
    basemoduleconfiguration.cpp \
    moduleactivist.cpp \
    basemodule.cpp \
    measmodeinfo.cpp \
    basemeaschannel.cpp \
    movingwindowfilter.cpp \
    basesamplechannel.cpp \
    basedspmeasprogram.cpp \
    boolvalidator.cpp \
    intvalidator.cpp \
    stringvalidator.cpp \
    doublevalidator.cpp \
    basemeasworkprogram.cpp \
    modulevalidator.cpp \
    veinmodulecomponent.cpp \
    veinmoduleparameter.cpp \
    veinmodulemetadata.cpp \
    veinmoduleactvalue.cpp \
    scpiinfo.cpp \
    basemeasmodule.cpp \
    basemoduleeventsystem.cpp


