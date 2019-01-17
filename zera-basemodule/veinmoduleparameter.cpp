#include "veinmoduleparameter.h"
#include "paramvalidator.h"
#include "scpiinfo.h"


cVeinModuleParameter::cVeinModuleParameter(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval, bool deferredNotification)
    :cVeinModuleComponent(entityId, eventsystem, name, description, initval), m_bDeferredNotification(deferredNotification)
{
    m_pValidator = 0;
}


cVeinModuleParameter::~cVeinModuleParameter()
{
    if (m_pValidator)
        delete m_pValidator;
    if (m_pscpiInfo)
        delete m_pscpiInfo;
}


bool cVeinModuleParameter::hasDeferredNotification()
{
    return m_bDeferredNotification;
}


bool cVeinModuleParameter::isValidParameter(QVariant& value)
{
    if (m_pValidator != 0)
        return m_pValidator->isValidParam(value);
    else
        return false;
}


void cVeinModuleParameter::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;

    jsonObj.insert("Description", m_sDescription);
    if (!m_sChannelName.isEmpty())
        jsonObj.insert("ChannelName", m_sChannelName);
    if (!m_sChannelUnit.isEmpty())
        jsonObj.insert("Unit", m_sChannelUnit);

    if (m_pValidator != 0)
    {
        QJsonObject jsonObj2;
        m_pValidator->exportMetaData(jsonObj2);
        jsonObj.insert("Validation", jsonObj2);
    }
    jsObj.insert(m_sName, jsonObj);
}


void cVeinModuleParameter::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_pscpiInfo)
        m_pscpiInfo->appendSCPIInfo(jsArr);
}


void cVeinModuleParameter::setSCPIInfo(cSCPIInfo *scpiinfo)
{
    m_pscpiInfo = scpiinfo;
}


void cVeinModuleParameter::setValidator(cParamValidator *validator)
{
    m_pValidator = validator;
}


void cVeinModuleParameter::transaction(QVariant newValue)
{
    emit sigValueChanged(newValue);
}


void cVeinModuleParameter::transaction(QUuid clientId, QVariant newValue)
{
    mClientIdList.append(clientId); // in case of deferred notification the event was accepted by the module validator
    emit sigValueChanged(newValue); // otherwise a notification will be sent later ... so we memorize the client id here
}
