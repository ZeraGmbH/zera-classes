#-------------------------------------------------
#
# Project created by QtCreator 2013-09-04T14:38:09
#
#-------------------------------------------------

# see user defined paths
include(power1module.user.pri)

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


TARGET = $$qtLibraryTarget(power1module)
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += "src"

SOURCES += \
    src/power1module.cpp \
    src/power1moduleconfiguration.cpp \
    src/power1modulefactory.cpp \
    src/power1modulemeasprogram.cpp \
    src/power1moduleobservation.cpp \

HEADERS += \
    src/debug.h \
    src/power1module.h \
    src/power1moduleconfigdata.h \
    src/power1moduleconfiguration.h \
    src/power1modulefactory.h \
    src/power1modulemeasprogram.h \
    src/power1moduleobservation.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

OTHER_FILES += \
    src/power1module.xml \
    src/power1module.xsd \
    src/power1module2.xml \
    src/power1module3.xml

config_files.files = $$OTHER_FILES
config_files.path = /etc/zera/modules


INSTALLS += config_files
