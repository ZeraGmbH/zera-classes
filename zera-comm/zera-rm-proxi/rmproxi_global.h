#ifndef RMPROXI_GLOBAL_H
#define RMPROXI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZERA_RMPROXI_LIBRARY)
#  define RMPROXISHARED_EXPORT Q_DECL_EXPORT
#else
#  define RMPROXISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RMPROXI_GLOBAL_H
