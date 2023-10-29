#include "jsonparamvalidator.h"
#include <QFile>

cJsonParamValidator::cJsonParamValidator()
{
}

void cJsonParamValidator::setJSonParameterStructure(const QJsonObject& jsonParamStructure)
{
    m_paramState.setStructure(jsonParamStructure);
}

bool cJsonParamValidator::isValidParam(QVariant &newValue)
{
    bool valid = false;
    QJsonObject jsonObj = newValue.toJsonObject();
    ZeraJsonParamsState::ErrList errList = m_paramState.validateJsonState(jsonObj);
    valid = errList.isEmpty();
    if(!valid) {
        while(!errList.isEmpty()) {
            ZeraJsonParamsState::errEntry err = errList.takeFirst();
            qWarning("%s: %s", qPrintable(err.strID()), qPrintable(err.m_strInfo));
        }
    }
    return valid;
}

void cJsonParamValidator::exportMetaData(QJsonObject &jsObj)
{
    // For now we don't touch. Later, once we gathered more background
    // information of how this works together with SCPI, we'll take care
    Q_UNUSED(jsObj)
}

