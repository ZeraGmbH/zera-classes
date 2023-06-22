#ifndef MODMAN_UTIL_H
#define MODMAN_UTIL_H

#include <functional>
#include <QHash>
#include <QString>
#include <QUuid>
#include <QVariantMap>

namespace modman_util
{
/// @b used to avoid non constexpr strlen use from QLatin1String(const char[]) constructor
/// @warning do not use \0 in the string literal, strlen("A\0BC") != string_literal_length("A\0BC")
template< size_t N >
constexpr size_t string_literal_length( char const (&)[N] )
{
    return N-1;
}

template < typename T>
constexpr QLatin1String to_latin1(T t_string) = delete;
//specialization to copy from other constexpr QLatin1String
template<>
constexpr QLatin1String to_latin1(const QLatin1String t_string) { return t_string; }
//overload for string literals
template<size_t N>
constexpr QLatin1String to_latin1(const char(&t_string)[N]) { return QLatin1String(t_string, string_literal_length(t_string)); }
}

/// helper to save time with defining component metadata
#define VF_COMPONENT(componentIdentifier, componentNameString, componentDescriptionString) \
    static constexpr QLatin1String s_##componentIdentifier##ComponentName = modman_util::to_latin1(componentNameString); \
    static constexpr QLatin1String s_##componentIdentifier##ComponentDescription = modman_util::to_latin1(componentDescriptionString);

/// helper to save time with defining rpc function metadata
#define VF_RPC(rpcIdentifier, procedureNameString, procedureDescriptionString) \
    static constexpr QLatin1String s_##rpcIdentifier##ProcedureName = modman_util::to_latin1(procedureNameString); \
    static constexpr QLatin1String s_##rpcIdentifier##ProcedureDescription = modman_util::to_latin1(procedureDescriptionString);

/// helper to set up a map or hash of the remote procedures, mainly because qt creator breaks the indentation :(
/// for infos on std::bind see https://en.cppreference.com/w/cpp/utility/functional/bind
#define VF_RPC_BIND(rpcIdentifier, rpcBind) \
{ \
    s_##rpcIdentifier##ProcedureName, \
    rpcBind \
    }

namespace VeinEvent
{
using RoutedRemoteProcedureAtlas = QHash<QString, std::function<void(const QUuid&, QVariantMap)> >;
}

#endif // MODMAN_UTIL_H
