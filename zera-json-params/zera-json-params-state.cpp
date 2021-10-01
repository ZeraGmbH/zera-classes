#include "zera-json-params-state.h"
#include "zera-json-params-structure.h"
#include "zera-json-export.h"

cZeraJsonParamsState::cZeraJsonParamsState()
{
}

bool cZeraJsonParamsState::isValid()
{
    return !m_jsonObjParamState.isEmpty();
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::loadJson(const QByteArray &jsonData, cZeraJsonParamsStructure *paramStructure, const QString &errHint)
{
    ErrList errList;
    if(paramStructure && paramStructure->isValid()) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        if(jsonDoc.isNull() || !jsonDoc.isObject()) {
            errEntry error(ERR_INVALID_JSON, errHint.isEmpty() ? jsonData : errHint);
            errList.push_back(error);
        }
        // TODO: let structure validate state data
    }
    else {
        errEntry error(ERR_INVALID_STRUCTURE, errHint.isEmpty() ? jsonData : errHint);
        errList.push_back(error);
    }
    return errList;
}

QByteArray cZeraJsonParamsState::exportJson(QJsonDocument::JsonFormat format)
{
    return cJsonExport::exportJson(m_jsonObjParamState, format);
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::param(const QStringList &paramPath, QVariant& value)
{
    ErrList errList;
    // TODO
    return errList;
}

cZeraJsonParamsState::ErrList cZeraJsonParamsState::setParam(const QStringList &paramPath, QVariant value)
{
    ErrList errList;
    // TODO
    return errList;
}


cZeraJsonParamsState::errEntry::errEntry(cZeraJsonParamsState::errorTypes errType, QString strInfo) :
    m_errType(errType),
    m_strInfo(strInfo)
{
}
