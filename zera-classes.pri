contains(DEFINES, OE_BUILD) {
  message(Openembedded build)
}
else {
  # paths for s.stirtzel work environment
  exists(/work/qt_projects/include/project-paths.pri) {
    include(/work/qt_projects/include/project-paths.pri)
  }

  exists($$PWD/autobuilder.pri) {
    include($$PWD/autobuilder.pri)
  }

  exists(/home/peter/C++/zera-classes-project/project-paths.pri) {
    include(/home/peter/C++/zera-classes-project/project-paths.pri)
  }
}


## Usage: TEMPLATE and in plugin cases CONFIG need to be defined before the include line for this .pri

LIBS += -L$$PWD/libs

#to be evaluated later: -Wshadow
QMAKE_CXXFLAGS += -Wall

contains( CONFIG, plugin ) {
  unix:equals(TEMPLATE, "lib") {
    DESTDIR = $$PWD/modules

    target.path = /usr/lib/zera-modules
    INSTALLS += target

    config_files.files = $$OTHER_FILES
    config_files.path = /etc/zera/modules
    INSTALLS += config_files
  }
} else:unix:equals(TEMPLATE, "lib") {
  DESTDIR = $$PWD/libs
  VERSION = 0.0.1

  target.path = /usr/lib
  INSTALLS += target
}

unix:equals(TEMPLATE, "staticlib") {
  DESTDIR = $$PWD/libs
}

### Currently no apps here
#unix:equals(TEMPLATE, "app") {
#  DESTDIR = $$PWD/bin
#
#  target.path = /usr/bin
#  INSTALLS += target
#}



## Usage: ZDEPENDS removes redundancy in dependency resolving, declare it before the line including this .pri

#Adds dependencies to protonet
contains( ZDEPENDS, protonet) {
  INCLUDEPATH += $$PROTONET_INCLUDEDIR
  LIBS += $$PROTONET_LIBDIR
  LIBS +=  -lproto-net-qt
}
#Adds dependencies to libvein
contains( ZDEPENDS, libvein) {
  INCLUDEPATH += $$VEIN_INCLUDEDIR
  LIBS += $$VEIN_LIBDIR
  LIBS +=  -lvein-qt
}
#Adds dependencies to the resourcemanager protobuf
contains( ZDEPENDS, resman) {
  INCLUDEPATH += $$RESOURCE_PROTOBUF_INCLUDEDIR
  LIBS += $$RESOURCE_PROTOBUF_LIBDIR
  LIBS +=  -lzera-resourcemanager-protobuf
  LIBS +=  -lprotobuf
}
#Adds dependencies to the SCPI
contains( ZDEPENDS, SCPI) {
  INCLUDEPATH += $$SCPI_INCLUDEDIR
  LIBS += $$SCPI_LIBDIR
  LIBS +=  -lSCPI
}
