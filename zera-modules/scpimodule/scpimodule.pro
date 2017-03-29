#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin

ZDEPENDS += SCPI

QT       += network core serialport

#LIBS +=  -lSCPI
LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

include(../zera-modules.pri)

INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../interface

TARGET = $$qtLibraryTarget(scpimodule)

INCLUDEPATH += "src"

SOURCES += \
    src/scpimodulefactory.cpp \
    src/scpimoduleconfiguration.cpp \
    src/scpimodule.cpp \
    src/scpidelegate.cpp \
    src/scpiserver.cpp \
    src/scpiinterface.cpp \
    src/scpimeasuredelegate.cpp \
    src/scpimeasure.cpp \
    src/scpiclient.cpp \
    src/scpiparameterdelegate.cpp \
    src/scpiinterfacedelegate.cpp \
    src/scpistatus.cpp \
    src/ieee488-2.cpp \
    src/baseinterface.cpp \
    src/moduleinterface.cpp \
    src/interfaceinterface.cpp \
    src/scpistatusdelegate.cpp \
    src/statusinterface.cpp \
    src/ieee4882interface.cpp \
    src/signalconnectiondelegate.cpp \
    src/scpiethclient.cpp \
    src/scpiserialclient.cpp \
    src/scpieventsystem.cpp \
    src/scpipropertydelegate.cpp \
    src/scpiclientinfo.cpp \
    src/scpicmdinfo.cpp

HEADERS += \
    src/scpimodulefactory.h \
    src/scpimodule.h \
    src/scpimoduleconfiguration.h \
    src/scpimoduleconfigdata.h \
    src/debug.h \
    src/scpidelegate.h \
    src/scpiserver.h \
    src/scpiinterface.h \
    src/scpicmdinfo.h \
    src/scpimeasuredelegate.h \
    src/scpimeasure.h \
    src/scpiclient.h \
    src/scpistatus.h \
    src/scpiparameterdelegate.h \
    src/scpiinterfacedelegate.h \
    src/ieee488-2.h \
    src/baseinterface.h \
    src/moduleinterface.h \
    src/interfaceinterface.h \
    src/scpistatusdelegate.h \
    src/statusinterface.h \
    src/ieee4882interface.h \
    src/signalconnectiondelegate.h \
    src/scpiethclient.h \
    src/scpiserialclient.h \
    src/statusbitdescriptor.h \
    src/scpieventsystem.h \
    src/scpipropertydelegate.h \
    src/scpiclientinfo.h \
    src/scpiethclient.h \
    src/scpiserialclient.h \
    src/statusbitdescriptor.h \
    src/scpieventsystem.h \
    src/scpipropertydelegate.h \
    src/scpiclientinfo.h

OTHER_FILES += \
    src/scpimodule.xml \
    src/scpimodule.xsd

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
