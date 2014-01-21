#ifndef RMPROXI_GLOBAL_H
#define RMPROXI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZERA_RMPROXY_LIBRARY)
#  define RMPROXYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RMPROXYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RMPROXI_GLOBAL_H
