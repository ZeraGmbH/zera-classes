#ifndef DEV_GLOBAL_H
#define DEV_GLOBAL_H

#include <QtCore/qglobal.h>

/**
  @mainpage ZERA_DEV library holds the stuff for device drivers used with Zera devices.
  @author Peter Lohmer p.lohmer@zera.de
*/


#if defined(ZERA_DEV_LIBRARY)
#  define ZERA_DEVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_DEVSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // DEV_GLOBAL_H
