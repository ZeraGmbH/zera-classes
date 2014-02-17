#ifndef ZERAMISC_GLOBAL_H
#define ZERAMISC_GLOBAL_H

#include <QtCore/qglobal.h>


/**
  @mainpage ZERA_MISC library holds frequently used algorithms used within Zera devices.
  @author Peter Lohmer p.lohmer@zera.de
*/


#if defined(ZERAMISC_LIBRARY)
#  define ZERA_MISCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_MISCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZERAMISC_GLOBAL_H
