#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(thdnmodule.user.pri)

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


TARGET = $$qtLibraryTarget(thdnmodule)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/thdnmodule.cpp \
    src/thdnmoduleconfiguration.cpp \
    src/thdnmodulefactory.cpp \
    src/thdnmodulemeasprogram.cpp \
    src/thdnmoduleobservation.cpp

HEADERS += \
    src/thdnmodule.h \
    src/thdnmoduleconfigdata.h \
    src/thdnmoduleconfiguration.h \
    src/thdnmodulefactory.h \
    src/thdnmodulemeasprogram.h \
    src/thdnmoduleobservation.h \
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
    src/thdnmodule.xml \
    src/thdnmodule.xsd

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
