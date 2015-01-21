#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(referencemodule.user.pri)

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


TARGET = $$qtLibraryTarget(referencemodule)
TEMPLATE = lib
CONFIG += plugin

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

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/referencemodule.xml \
    src/referencemodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
