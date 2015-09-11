#include "veinmoduleactvalue.h"
#include "scpiinfo.h"

cVeinModuleActvalue::cVeinModuleActvalue(int entityId,  VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval)
    :cVeinModuleComponent(entityId, eventsystem, name, description, initval)
{
    m_pscpiInfo = 0;
}


cVeinModuleActvalue::~cVeinModuleActvalue()
{
    if (m_pscpiInfo)
        delete m_pscpiInfo;
}


void cVeinModuleActvalue::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_pscpiInfo)
        m_pscpiInfo->appendSCPIInfo(jsArr);
}


void cVeinModuleActvalue::setSCPIInfo(cSCPIInfo *scpiinfo)
{
    m_pscpiInfo = scpiinfo;
}
