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


TARGET = $$qtLibraryTarget(lambdamodule)


INCLUDEPATH += "src"

SOURCES += \
    src/lambdamodule.cpp \
    src/lambdamoduleconfiguration.cpp \
    src/lambdamodulefactory.cpp \
    src/lambdamodulemeasprogram.cpp \
    src/lambdameasdelegate.cpp

HEADERS += \
    src/debug.h \
    src/lambdamodule.h \
    src/lambdamoduleconfigdata.h \
    src/lambdamoduleconfiguration.h \
    src/lambdamodulefactory.h \
    src/lambdamodulemeasprogram.h \
    src/lambdameasdelegate.h

OTHER_FILES += \
    src/lambdamodule.xsd \
    src/com5003-lambdamodule.xml \
    src/mt310s2-lambdamodule.xml


RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)


