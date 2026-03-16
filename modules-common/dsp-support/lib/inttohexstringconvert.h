#ifndef INTTOHEXSTRINGCONVERT_H
#define INTTOHEXSTRINGCONVERT_H

#include <QString>

class IntToHexStringConvert
{
public:
    static QString convert(quint32 value, int minDigits=4, bool with0x=true);
};

#endif // INTTOHEXSTRINGCONVERT_H
