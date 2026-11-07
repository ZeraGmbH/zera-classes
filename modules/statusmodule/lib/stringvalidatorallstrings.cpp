#include "stringvalidatorallstrings.h"
#include <QJsonArray>

bool StringValidatorAllStrings::isValidParam(QVariant &newValue)
{
    Q_UNUSED(newValue)
    return true;
}

void StringValidatorAllStrings::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "STRING");
    jsObj.insert("Data", QJsonArray());
}
