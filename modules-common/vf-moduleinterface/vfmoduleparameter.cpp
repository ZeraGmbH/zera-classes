#include "vfmoduleparameter.h"
#include "validatorinterface.h"
#include "scpiinfo.h"

VfModuleParameter::VfModuleParameter(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval, bool deferredNotification, bool deferredQueryNotification)
    :VfModuleComponent(entityId, eventsystem, name, description, initval),
      m_bDeferredNotification(deferredNotification),
      m_bDeferredQueryNotification(deferredQueryNotification),
      m_pValidator(nullptr),
      m_pscpiInfo(nullptr)
{
}

VfModuleParameter::~VfModuleParameter()
{
    delete m_pValidator;
    delete m_pscpiInfo;
}

bool VfModuleParameter::hasDeferredNotification()
{
    return m_bDeferredNotification;
}

bool VfModuleParameter::hasDeferredQueryNotification()
{
    return m_bDeferredQueryNotification;
}

bool VfModuleParameter::isValidParameter(QVariant& value)
{
    if (m_pValidator != 0) {
        return m_pValidator->isValidParam(value);
    }
    else {
        return false;
    }
}

void VfModuleParameter::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;
    jsonObj.insert("Description", m_sDescription);
    if (!m_sChannelName.isEmpty()) {
        jsonObj.insert("ChannelName", m_sChannelName);
    }
    if (!m_sChannelUnit.isEmpty()) {
        jsonObj.insert("Unit", m_sChannelUnit);
    }
    if (m_pValidator != nullptr) {
        QJsonObject jsonObj2;
        m_pValidator->exportMetaData(jsonObj2);
        jsonObj.insert("Validation", jsonObj2);
    }
    jsObj.insert(m_sName, jsonObj);
}

void VfModuleParameter::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_pscpiInfo) {
        m_pscpiInfo->appendSCPIInfo(jsArr);
    }
}

void VfModuleParameter::setSCPIInfo(cSCPIInfo *scpiinfo)
{
    m_pscpiInfo = scpiinfo;
}

void VfModuleParameter::setValidator(ValidatorInterface *validator)
{
    if(m_pValidator)
        delete m_pValidator;
    m_pValidator = validator;
}

void VfModuleParameter::veinTransaction(QUuid clientId, QVariant newValue, QVariant oldValue, VeinComponent::ComponentData::Command vccmd)
{
    mClientIdList.append(clientId);
    if (vccmd == VeinComponent::ComponentData::Command::CCMD_FETCH) {
        if (hasDeferredQueryNotification()) {
            emit sigValueQuery(newValue);
        }
        else {
            setValue(oldValue); // this will send notification event
        }
    }
    else {
        if (isValidParameter(newValue)) {
            if (!hasDeferredNotification()) {
                setValue(newValue); // in case of no deferred if send the notification at once
            }
            emit sigValueChanged(newValue);
        }
        else {
            setError(); // this will send an error notification
        }
    }
}
