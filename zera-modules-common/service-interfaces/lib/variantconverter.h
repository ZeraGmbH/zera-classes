#ifndef VARIANTCONVERTER_H
#define VARIANTCONVERTER_H

#include <QVariant>

namespace Zera {

class VariantConverter
{
public:
    static QVariant returnInt(QString message);
    static QVariant returnString(QString message);
    static QVariant returnStringList(QString message);
    static QVariant returnDouble(QString message);
    static QVariant returnBool(QString message);
};

}
#endif // VARIANTCONVERTER_H
