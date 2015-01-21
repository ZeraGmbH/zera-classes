#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(rangemodule.user.pri)

QT       += network core
QT       -= gui

LIBS +=  -lzera-proxy
LIBS +=  -lzeramisc
LIBS +=  -lzeraxmlconfig
LIBS +=  -lvein-qt
LIBS +=  -lzerabasemodule
//LIBS +=  -L$$shadowed($$PWD/../../libs)


INCLUDEPATH += ../../zera-basemodule \
    ../../zera-xml-config/src \
    ../../zera-misc \
    ../../zera-comm/zera-proxy \


TARGET = $$qtLibraryTarget(rangemodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/rangemodulefactory.cpp \
    src/rangemoduleconfiguration.cpp \
    src/rangemeaschannel.cpp \
    src/rangemodulemeasprogram.cpp \
    src/rangeobsermatic.cpp \
    src/adjustment.cpp \
    src/rangemoduleobservation.cpp \
    src/rangemodule.cpp

HEADERS += \
    src/rangemodulefactory.h \
    src/rangemodule.h \
    src/rangemoduleconfiguration.h \
    src/rangemoduleconfigdata.h \
    src/rangemeaschannel.h \
    src/rangemodulemeasprogram.h \
    src/rangeobsermatic.h \
    src/adjustment.h \
    src/rangemoduleobservation.h \
    src/debug.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/rangemoduleREF.xml \
    src/rangemodule.xml \
    src/rangemodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
