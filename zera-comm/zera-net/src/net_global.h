#ifndef ZERA_NET_GLOBAL_H
#define ZERA_NET_GLOBAL_H

#include <QtCore/qglobal.h>

/*! \mainpage Zera networking class-library
 *
 * \section Introduction
 *
 * The Zera network library is part of the Zera class-libraries,
 * it provides a standardised general purpose network stack.
 *
 * \section Usage
 *
 * \subsection Examples:
 *
 */

#if defined(ZERA_NET_LIBRARY)
#  define ZERA_NETSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZERA_NETSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZERA_NET_GLOBAL_H
