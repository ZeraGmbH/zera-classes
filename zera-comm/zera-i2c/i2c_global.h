#ifndef I2C_GLOBAL_H
#define I2C_GLOBAL_H

#include <QtCore/qglobal.h>

/**
  @mainpage ZERA_I2C library holds the stuff for i2c communication used with Zera devices.
  @author Peter Lohmer p.lohmer@zera.de
*/


#if defined(ZERA_I2C_LIBRARY)
#  define ZERA_I2CSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_I2CSHARED_EXPORT Q_DECL_IMPORT
#endif



#endif // I2C_GLOBAL_H
