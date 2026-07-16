#include "vfmodulecomponentcreator.h"

VfModuleComponentCreator::VfModuleComponentCreator(int entityId, VeinEvent::EventSystem *eventsystem) :
    m_entityId(entityId),
    m_eventsystem(eventsystem)
{
}

VfModuleComponent *VfModuleComponentCreator::createComponent(const QString &componentName, const QString &description,
                                                             const QString &channelName, const QString &unit,
                                                             const QString &scpiModel, const QString &scpiCmd, int scpiQueryCmdFlagsMask,
                                                             const QVariant &initval) const
{
    VfModuleComponent *component = new VfModuleComponent(m_entityId,
                                                         m_eventsystem,
                                                         componentName,
                                                         description,
                                                         initval);
    component->setChannelName(channelName);
    component->setUnit(unit);
    if (!scpiModel.isEmpty() && !scpiCmd.isEmpty())
        component->setScpiInfo(scpiModel, scpiCmd, scpiQueryCmdFlagsMask);
    return component;
}
