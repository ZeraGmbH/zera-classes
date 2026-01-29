#include "vfmoduleparameter.h"
#include "validatorinterface.h"

VfModuleParameter::VfModuleParameter(int entityId,
                                     VeinEvent::EventSystem *eventsystem,
                                     const QString &componentname,
                                     const QString &description,
                                     const QVariant &initval,
                                     bool deferredNotification) :
    VfModuleComponent(entityId, eventsystem, componentname, description, initval),
    m_bDeferredNotification(deferredNotification),
    m_pValidator(nullptr)
{
}

VfModuleParameter::~VfModuleParameter()
{
    delete m_pValidator;
}

bool VfModuleParameter::isValidParameter(QVariant& value)
{
    if (m_pValidator != 0)
        return m_pValidator->isValidParam(value);
    return false;
}

void VfModuleParameter::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;
    jsonObj.insert("Description", m_sDescription);
    if (!m_sChannelName.isEmpty())
        jsonObj.insert("ChannelName", m_sChannelName);
    if (!m_sChannelUnit.isEmpty())
        jsonObj.insert("Unit", m_sChannelUnit);
    if (m_pValidator != nullptr) {
        QJsonObject jsonObj2;
        m_pValidator->exportMetaData(jsonObj2);
        jsonObj.insert("Validation", jsonObj2);
    }
    jsObj.insert(m_componentName, jsonObj);
}

void VfModuleParameter::setValidator(ValidatorInterface *validator)
{
    if(m_pValidator)
        delete m_pValidator;
    m_pValidator = validator;
}

void VfModuleParameter::handleNoFetchVeinTransaction(QVariant newValue)
{
    if (isValidParameter(newValue)) {
        if (!m_bDeferredNotification)
            setValue(newValue); // in case of no deferred if send the notification at once
        emit sigValueChanged(newValue);
    }
    else
        setError();
}

void VfModuleParameter::veinTransaction(QUuid clientId, QVariant newValue, QVariant oldValue, VeinComponent::ComponentData::Command vccmd)
{
    mClientIdList.append(clientId);
    if (vccmd == VeinComponent::ComponentData::Command::CCMD_FETCH)
        setValue(oldValue); // this will send notification event
    else
        handleNoFetchVeinTransaction(newValue); // this will send an error notification
}
