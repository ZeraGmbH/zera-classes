#ifndef SCPIDELEGATEXMLEXPORTGENERATOR_H
#define SCPIDELEGATEXMLEXPORTGENERATOR_H

#include "scpidelegatetemplate.h"
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE {

class ScpiDelegateXmlExportGenerator
{
public:
    static void setXmlComponentInfo(const ScpiBaseDelegatePtr &delegate, const QJsonObject &componentInfo);
    static void setXmlComponentValidatorInfo(const ScpiBaseDelegatePtr &delegate, const QJsonObject &componentInfo);
    static QJsonArray getValidatorEntries(const QJsonObject &validator);
};

}
#endif // SCPIDELEGATEXMLEXPORTGENERATOR_H
