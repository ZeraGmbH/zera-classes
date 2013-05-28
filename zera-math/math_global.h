#ifndef MATH_GLOBAL_H
#define MATH_GLOBAL_H

#include <QtCore/qglobal.h>

/**
  @mainpage ZERA_MATH library holds mathematical calculations used with Zera devices.
  @author Peter Lohmer p.lohmer@zera.de
*/


#if defined(ZERA_MATH_LIBRARY)
#  define ZERA_MATHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_MATHSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // MATH_GLOBAL_H
