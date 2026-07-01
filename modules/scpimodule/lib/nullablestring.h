#ifndef NULLABLESTRING_H
#define NULLABLESTRING_H

#include <QString>
#include <QList>

class NullableString
{
public:
    NullableString();
    NullableString(const QString &str);

    bool isNull() const;
    const QString &getStr() const;
    NullableString &operator= (const QString &str);
    bool operator== (const NullableString &other) const;

private:
    void setStr(const QString &str);

    QString m_str;
    bool m_isNull = true;
};

typedef QList<NullableString> NullableStringList;

#endif // NULLABLESTRING_H
