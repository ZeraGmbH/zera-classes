#include "veinmoduleactvalue.h"
#include "scpiinfo.h"

cVeinModuleActvalue::cVeinModuleActvalue(int entityId,  VeinEvent::EventSystem *eventsystem, QString name, QString description, QVariant initval)
    :cVeinModuleComponent(entityId, eventsystem, name, description, initval)
{
}


cVeinModuleActvalue::~cVeinModuleActvalue()
{
    if (m_pscpiInfo)
        delete m_pscpiInfo;
}


void cVeinModuleActvalue::exportSCPIInfo(QJsonObject &jsObj)
{
    if (m_pscpiInfo)
        m_pscpiInfo->appendSCPIInfo(jsObj);
}


void cVeinModuleActvalue::setSCPIInfo(cSCPIInfo *scpiinfo)
{
    m_pscpiInfo = scpiinfo;
}
