#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(dftmodule.user.pri)

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


TARGET = $$qtLibraryTarget(dftmodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/dftmodule.cpp \
    src/dftmodulefactory.cpp \
    src/dftmoduleconfiguration.cpp \
    src/dftmodulemeasprogram.cpp \
    src/dftmoduleobservation.cpp

HEADERS += \
    src/dftmodule.h \
    src/dftmoduleconfigdata.h \
    src/dftmoduleconfiguration.h \
    src/dftmodulefactory.h \
    src/dftmodulemeasprogram.h \
    src/dftmoduleobservation.h \
    src/debug.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/dftmodule.xml \
    src/dftmodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
