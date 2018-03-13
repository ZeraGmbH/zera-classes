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


TARGET = $$qtLibraryTarget(oscimodule)


INCLUDEPATH += "src"

SOURCES += \
    src/oscimodule.cpp \
    src/oscimoduleconfiguration.cpp \
    src/oscimodulefactory.cpp \
    src/oscimodulemeasprogram.cpp \
    src/oscimoduleobservation.cpp

HEADERS += \
    src/debug.h \
    src/oscimodule.h \
    src/oscimoduleconfigdata.h \
    src/oscimoduleconfiguration.h \
    src/oscimodulefactory.h \
    src/oscimodulemeasprogram.h \
    src/oscimoduleobservation.h

OTHER_FILES += \
    src/oscimodule.xsd \
    src/com5003-oscimodule.xml \
    src/mt310s2-oscimodule.xml

RESOURCES += \
    resources.qrc

include(../../zera-classes.pri)


