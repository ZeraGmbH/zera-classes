#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += plugin

ZDEPENDS += libvein
include(../../zera-classes.pri)

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lzerabasemodule



INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \
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
    src/referencemodule.cpp \
    src/referencemoduleinit.cpp

HEADERS += \
    src/referencemodulefactory.h \
    src/referencemodule.h \
    src/referencemoduleconfiguration.h \
    src/referencemoduleconfigdata.h \
    src/referencemeaschannel.h \
    src/referencemodulemeasprogram.h \
    src/referenceadjustment.h \
    src/referencemoduleobservation.h \
    src/debug.h \
    src/referencemoduleinit.h

OTHER_FILES += \
    src/referencemodule.xml \
    src/referencemodule.xsd

RESOURCES += \
    resource.qrc
