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
LIBS +=  -lzeramath
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule
LIBS +=  -lMeasurementModuleInterface

include(../zera-modules.pri)

INCLUDEPATH += ../interface \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-math \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../../zera-basemodule


TARGET = $$qtLibraryTarget(adjustmentmodule)


INCLUDEPATH += "src"

SOURCES += \
    src/adjustmentmodule.cpp \
    src/adjustmentmoduleconfiguration.cpp \
    src/adjustmentmodulefactory.cpp \
    src/adjustmentmodulemeasprogram.cpp \
    src/adjustvalidator.cpp

HEADERS += \
    src/debug.h \
    src/adjustmentmodule.h \
    src/adjustmentmoduleconfigdata.h \
    src/adjustmentmoduleconfiguration.h \
    src/adjustmentmodulefactory.h \
    src/adjustmentmodulemeasprogram.h \
    src/adjustvalidator.h

OTHER_FILES += \
    src/adjustmentmodule.xsd \
    src/com5003-adjustmentmodule.xml \
    src/mt310s2-adjustmentmodule.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)

