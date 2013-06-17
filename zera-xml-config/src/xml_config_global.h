#ifndef XML_CONFIG_GLOBAL_H
#define XML_CONFIG_GLOBAL_H

#include <QtCore/qglobal.h>

/**
  @mainpage ZERA XML config library
*/


#if defined(ZERA_XML_CONFIG_LIBRARY)
#  define ZERA_XML_CONFIGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_XML_CONFIGSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // XML_CONFIG_GLOBAL_H
