#-------------------------------------------------
#
# Project created by QtCreator 2013-11-27T13:25:24
#
#-------------------------------------------------

QT       -= gui

#TARGET = MeasurementModuleInterface
TEMPLATE = lib
#CONFIG += staticlib

HEADERS += \
    virtualmodule.h \
    abstractmodulefactory.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/zera-modules
    } else {
        target.path = /usr/lib/zera-modules
    }
    INSTALLS += target
}

header_files.files = $$HEADERS
header_files.path = /usr/include/zera-modules
INSTALLS += header_files
