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

INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
    ../../zera-validator \
    ../interface


TARGET = $$qtLibraryTarget(referencemodule)


INCLUDEPATH += "src"

SOURCES += \
    src/referencemodulefactory.cpp \
    src/referencemoduleconfiguration.cpp \
    src/referencemeaschannel.cpp \
    src/referencemodulemeasprogram.cpp \
    src/referenceadjustment.cpp \
    src/referencemoduleobservation.cpp \
    src/referencemodule.cpp

HEADERS += \
    src/referencemodulefactory.h \
    src/referencemodule.h \
    src/referencemoduleconfiguration.h \
    src/referencemoduleconfigdata.h \
    src/referencemeaschannel.h \
    src/referencemodulemeasprogram.h \
    src/referenceadjustment.h \
    src/referencemoduleobservation.h \
    src/debug.h

OTHER_FILES += \
    src/referencemodule.xsd

RESOURCES += \
    resource.qrc

include(../../zera-classes.pri)

DISTFILES += \
    src/com5003-referencemodule.xml
