#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T16:48:29
#
#-------------------------------------------------
TEMPLATE = lib

ZDEPENDS += resman libvein zera-validator

QT       -= gui
QT       += network

TARGET = zerabasemodule

#dependencies
VEIN_DEP_EVENT = 1
VEIN_DEP_COMP = 1

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
    measmodeinfo.h \
    foutinfo.h \
    rangeinfo.h \
    basemeaschannel.h \
    movingwindowfilter.h \
    basesamplechannel.h \
    secinputinfo.h \
    basedspmeasprogram.h \
    ecalcinfo.h \
    interfaceentity.h \
    paramvalidator.h \
    boolvalidator.h \
    intvalidator.h \
    stringvalidator.h \
    doublevalidator.h \
<<<<<<< HEAD
    basemeasworkprogram.h
=======
    modulevalidator.h
>>>>>>> zera-basemodule: added modulevalidator

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
    basesamplechannel.cpp \
    basedspmeasprogram.cpp \
    interfaceentity.cpp \
    boolvalidator.cpp \
    intvalidator.cpp \
    stringvalidator.cpp \
    doublevalidator.cpp \
<<<<<<< HEAD
    basemeasworkprogram.cpp
=======
    modulevalidator.cpp
>>>>>>> zera-basemodule: added modulevalidator

include(../zera-classes.pri)
