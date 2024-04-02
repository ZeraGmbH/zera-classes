#include "vfmoduleactvalue.h"
#include "scpiinfo.h"

VfModuleActvalue::VfModuleActvalue(int entityId,  VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval)
    :VfModuleComponent(entityId, eventsystem, name, description, initval)
{
    m_pscpiInfo = 0;
}

VfModuleActvalue::~VfModuleActvalue()
{
    if (m_pscpiInfo) {
        delete m_pscpiInfo;
    }
}

void VfModuleActvalue::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_pscpiInfo) {
        m_pscpiInfo->appendSCPIInfo(jsArr);
    }
}

void VfModuleActvalue::setSCPIInfo(cSCPIInfo *scpiinfo)
{
    m_pscpiInfo = scpiinfo;
}
