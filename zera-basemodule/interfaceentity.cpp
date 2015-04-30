#include <QJsonArray>
#include <QJsonObject>

#include "interfaceentity.h"


cInterfaceEntity::cInterfaceEntity(QString name, QString des, QString valscpript, QString valpar, QString model, QString cmdnode, QString type, QString unit)
    :m_sEntityName(name), m_sDescription(des), m_sValScriptName(valscpript), m_sValParameter(valpar), m_sSCPIModel(model), m_sSCPICmdnode(cmdnode), m_sSCPIType(type), m_sUnit(unit)
{
}


void cInterfaceEntity::setName(QString name)
{
    m_sEntityName = name;
}


void cInterfaceEntity::setDescription(QString des)
{
    m_sDescription = des;
}


void cInterfaceEntity::setValidationScript(QString script)
{
    m_sValScriptName = script;
}


void cInterfaceEntity::setValidationParamter(QString par)
{
    m_sValParameter = par;
}


void cInterfaceEntity::setSCPIModel(QString model)
{
    m_sSCPIModel = model;
}


void cInterfaceEntity::setSCPICmdnode(QString node)
{
    m_sSCPICmdnode = node;
}


void cInterfaceEntity::setSCPIType(QString type)
{
    m_sSCPIType = type;
}


void cInterfaceEntity::setUnit(QString unit)
{
    m_sUnit = unit;
}


void cInterfaceEntity::appendInterfaceEntity(QJsonArray &jsArr)
{
    QJsonObject jsonObj;
    jsonObj.insert("Name", m_sEntityName);
    jsonObj.insert("DES", m_sDescription);

    QJsonArray jsonValArr;
    jsonValArr.append(m_sValScriptName); // we don't need validation for queries
    jsonValArr.append(m_sValParameter);

    jsonObj.insert("VAL", jsonValArr);

    QJsonArray jsonSCPIArr;

    jsonSCPIArr.append(m_sSCPIModel);
    jsonSCPIArr.append(m_sSCPICmdnode);

    jsonSCPIArr.append(QString(m_sSCPIType));
    jsonSCPIArr.append(m_sUnit);

    jsonObj.insert("SCPI", jsonSCPIArr);

    jsArr.append(jsonObj);
}


