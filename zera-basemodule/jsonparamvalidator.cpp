#include "jsonparamvalidator.h"
#include <complex.h>
#include <zera-json-params-structure.h>
#include <QFile>
#include <QJsonDocument>

cJsonParamValidator::cJsonParamValidator()
{

}

bool cJsonParamValidator::isValidParam(QVariant &newValue)
{
    bool valid = false;
    if(m_pParamStructure) {
        QJsonObject jsonObj = QJsonDocument::fromJson(newValue.toByteArray()).object(); // hope we can ged rid of this
        cZeraJsonParamsStructure::ErrList errList = m_pParamStructure->validateJsonState(jsonObj);
        valid = errList.isEmpty();
        if(!valid) {
            while(!errList.isEmpty()) {
                cZeraJsonParamsStructure::errEntry err = errList.takeFirst();
                qWarning("%s: %s", qPrintable(err.strID()), qPrintable(err.m_strInfo));
            }
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

void cJsonParamValidator::setJSonParameterState(cZeraJsonParamsStructure *pParamStructure)
{
    m_pParamStructure = pParamStructure;
}

