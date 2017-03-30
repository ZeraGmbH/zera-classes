#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += plugin

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

include(../zera-modules.pri)

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(lamdamodule)


INCLUDEPATH += "src"

SOURCES += \
    src/lamdamodule.cpp \
    src/lamdamoduleconfiguration.cpp \
    src/lamdamodulefactory.cpp \
    src/lamdamodulemeasprogram.cpp \
    src/lamdameasdelegate.cpp

HEADERS += \
    src/debug.h \
    src/lamdamodule.h \
    src/lamdamoduleconfigdata.h \
    src/lamdamoduleconfiguration.h \
    src/lamdamodulefactory.h \
    src/lamdamodulemeasprogram.h \
    src/lamdameasdelegate.h

OTHER_FILES += \
    src/lamdamodule.xml \
    src/lamdamodule.xsd


RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)
