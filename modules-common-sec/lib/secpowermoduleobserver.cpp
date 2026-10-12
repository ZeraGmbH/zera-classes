#include "secpowermoduleobserver.h"

SecPowerModuleObserver::SecPowerModuleObserver(const VeinStorage::AbstractDatabase *veinDb) :
    m_veinDb(veinDb)
{
}

void SecPowerModuleObserver::setPowerModulentityId(int entityId)
{
    if (m_currentPowerModuleEntityId != entityId) {
        m_currentPowerModuleEntityId = entityId;
        if (m_veinDb) {
            m_measTypeComponent = m_veinDb->findComponent(m_currentPowerModuleEntityId, "ACT_PowerDisplayName");
            connect(m_measTypeComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, this, [&](const QVariant &newValue) {
                emit sigPowerModuleValuesChanged();
            });
            m_constantComponent = m_veinDb->findComponent(m_currentPowerModuleEntityId, "PAR_FOUTConstant0");
            connect(m_constantComponent.get(), &VeinStorage::AbstractComponent::sigValueChange, this, [&](const QVariant &newValue) {
                emit sigPowerModuleValuesChanged();
            });
        }
        emit sigPowerModuleValuesChanged();
    }
}

int SecPowerModuleObserver::getPowerModulentityId() const
{
    return m_currentPowerModuleEntityId;
}

QString SecPowerModuleObserver::getPowerType() const
{
    if (m_veinDb)
        return m_veinDb->getStoredValue(m_currentPowerModuleEntityId, "ACT_PowerDisplayName").toString();
    return "";
}

double SecPowerModuleObserver::getConstant() const
{
    if (m_veinDb)
        return m_veinDb->getStoredValue(m_currentPowerModuleEntityId, "PAR_FOUTConstant0").toDouble();
    return 0.0;
}
