#include "inttohexstringconvert.h"

QString IntToHexStringConvert::convert(quint32 value, int minDigits, bool with0x)
{
    QString str = QString("%1").arg(value, minDigits, 16, QChar('0'));
    str = str.toUpper();
    if(with0x)
        str = "0x" + str;
    return str;
}
