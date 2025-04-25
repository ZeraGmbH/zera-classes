#include "apimodulerequestvalidator.h"

namespace APIMODULE
{
cApiModuleRequestValidator::cApiModuleRequestValidator() {}

bool cApiModuleRequestValidator::isValidParam(QVariant &newValue)
{
    if (static_cast<QMetaType::Type>(newValue.type()) != QMetaType::QJsonObject && static_cast<QMetaType::Type>(newValue.type()) != QMetaType::QVariantMap)
        return false;

    QVariantMap newValueMap = newValue.value<QVariantMap>();
    if (newValueMap.size() != 3 ||
        !newValueMap.contains("Name") ||
        !newValueMap.contains("Type") ||
        !newValueMap.contains("Token")) {
        return false;
    }

    // Check "Name" and "Token" are strings
    const QVariant& nameVar = newValueMap["Name"];
    const QVariant& tokenVar = newValueMap["Token"];
    if (nameVar.type() != QVariant::String ||
        tokenVar.type() != QVariant::String) {
        return false;
    }

    // Check "Type" is numeric (int, double, etc.)
    const QVariant& typeVar = newValueMap["Type"];
    const int typeId = typeVar.type();
    switch (typeId) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Double:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Float:
        return true;
    default:
        return false;
    }
}

void cApiModuleRequestValidator::exportMetaData(QJsonObject &jsObj)
{
    Q_UNUSED(jsObj)
}
}
