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

cZeraJsonParamsState::ErrList cZeraJsonParamsState::loadState(QJsonObject& jsonState, cZeraJsonParamsStructure* paramStructure)
{
    ErrList errList;
    if(paramStructure && paramStructure->isValid()) {
        // TODO: let structure validate state data
    }
    else {
        errEntry error(ERR_INVALID_STRUCTURE, QString());
        errList.push_back(error);
    }
    return errList;
}

const QJsonObject &cZeraJsonParamsState::jsonState()
{

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

QString cZeraJsonParamsState::errEntry::strID()
{
    QString str;
    switch(m_errType) {
    case ERR_INVALID_STRUCTURE:
        str = "Invalid parameter structure";
    case ERR_INVALID_PARAM_PATH:
        str = "Invalid parameter path";
    case ERR_PARAM_DOES_NOT_EXIST:
        str = "Parameter does not exist";
    case ERR_INVALID_VALUE:
        str = "Invalid parameter value";
    }
    return str;
}
