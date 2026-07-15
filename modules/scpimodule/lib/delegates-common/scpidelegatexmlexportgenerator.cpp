#include "scpidelegatexmlexportgenerator.h"

namespace SCPIMODULE {

void ScpiDelegateXmlExportGenerator::setXmlComponentInfo(const ScpiBaseDelegatePtr &delegate, const QJsonObject &componentInfo)
{
    QString desc = componentInfo["Description"].toString();
    if(!desc.isEmpty())
        delegate->setXmlAttribute("Description", desc);
    setXmlComponentValidatorInfo(delegate, componentInfo);
    QString unit = componentInfo["Unit"].toString();
    if(!unit.isEmpty())
        delegate->setXmlAttribute("Unit", unit);
}

void ScpiDelegateXmlExportGenerator::setXmlComponentValidatorInfo(const ScpiBaseDelegatePtr &delegate, const QJsonObject &componentInfo)
{
    QJsonObject validator = componentInfo["Validation"].toObject();
    QString validatorType = validator["Type"].toString();
    if(!validatorType.isEmpty()) {
        if(validatorType == "STRING") {
            delegate->setXmlAttribute("DataType", validatorType);
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QStringList validStrings;
            for(const auto &entry : qAsConst(validatorEntryArray)) {
                validStrings.append(entry.toString());
            }
            if(!validStrings.isEmpty())
                delegate->setXmlAttribute("ValidPar", validStrings.join(","));
        }
        else if(validatorType == "BOOL") {
            delegate->setXmlAttribute("DataType", validatorType);
            delegate->setXmlAttribute("Min", QString("0"));
            delegate->setXmlAttribute("Max", QString("1"));
        }
        else if(validatorType == "INTEGER" || validatorType == "DOUBLE") {
            delegate->setXmlAttribute("DataType", validatorType);
            QJsonArray validatorEntryArray = getValidatorEntries(validator);
            QString minVal = validatorEntryArray[0].toVariant().toString();
            QString maxVal = validatorEntryArray[1].toVariant().toString();
            if(!minVal.isEmpty() && !maxVal.isEmpty()) {
                delegate->setXmlAttribute("Min", minVal);
                delegate->setXmlAttribute("Max", maxVal);
            }
        }
        else if(validatorType == "REGEX") {
            delegate->setXmlAttribute("DataType", "STRING");
            QString regEx = validator["Data"].toString();
            delegate->setXmlAttribute("RegEx", regEx);
        }
    }
}

QJsonArray ScpiDelegateXmlExportGenerator::getValidatorEntries(const QJsonObject &validator)
{
    return validator["Data"].toArray();
}

}
