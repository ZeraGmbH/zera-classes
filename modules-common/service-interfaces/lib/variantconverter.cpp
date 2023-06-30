#include "variantconverter.h"

namespace Zera {

QVariant VariantConverter::returnInt(QString message)
{
    return QVariant(message.toInt());
}

QVariant VariantConverter::returnString(QString message)
{
    return QVariant(message);
}

QVariant VariantConverter::returnStringList(QString message)
{
    return QVariant(message.split(";"));
}

QVariant VariantConverter::returnDouble(QString message)
{
    return QVariant(message.toDouble());
}

QVariant VariantConverter::returnBool(QString message)
{
    return QVariant(message.toInt() == 1);
}

}
