#include "nullablestring.h"

NullableString::NullableString()
{
}

NullableString::NullableString(const QString &str)
{
    setStr(str);
}

bool NullableString::isNull() const
{
    return m_isNull;
}

const QString &NullableString::getStr() const
{
    return m_str;
}

NullableString &NullableString::operator=(const QString &nullableStr)
{
    setStr(nullableStr);
    return *this;
}

bool NullableString::operator==(const NullableString &other) const
{
    return m_isNull == other.m_isNull && m_str == other.m_str;
}

void NullableString::setStr(const QString &str)
{
    m_str = str;
    m_isNull = false;
}
