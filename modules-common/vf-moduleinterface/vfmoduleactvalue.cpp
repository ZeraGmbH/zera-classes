#include "vfmoduleactvalue.h"

VfModuleActvalue::VfModuleActvalue(int entityId,
                                   VeinEvent::EventSystem *eventsystem,
                                   QString name,
                                   QString description,
                                   QVariant initval) :
    VfModuleComponent(entityId, eventsystem, name, description, initval)
{
}

void VfModuleActvalue::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_scpiInfo)
        m_scpiInfo->appendSCPIInfo(jsArr);
}

void VfModuleActvalue::setScpiInfo(const QString &model,
                                   const QString &cmd,
                                   int cmdTypeMask,
                                   const QString &veinComponentName,
                                   SCPI::eSCPIEntryType entryType)
{
    m_scpiInfo = std::make_unique<ScpiVeinComponentInfo>(model,
                                                         cmd,
                                                         cmdTypeMask,
                                                         veinComponentName,
                                                         entryType);
}
