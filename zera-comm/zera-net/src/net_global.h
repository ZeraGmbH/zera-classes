#ifndef ZERA_NET_GLOBAL_H
#define ZERA_NET_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZERA_NET_LIBRARY)
#  define ZERA_NETSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_NETSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZERA_NET_GLOBAL_H
