#include "vfmodulecomponentstoragefetchonly.h"
#include <scpi.h>

VfModuleComponentStorageFetchOnly::VfModuleComponentStorageFetchOnly(int entityId,
                                                                     const QString &componentName,
                                                                     const QString &description) :
    m_entityId(entityId),
    m_componentName(componentName),
    m_description(description)
{
}

void VfModuleComponentStorageFetchOnly::setScpiInfo(const QString &model, const QString &cmd)
{
    m_scpiInfo = std::make_unique<VfModuleMetaInfoContainer>(model,
                                                             cmd,
                                                             SCPI::isQuery,
                                                             m_componentName,
                                                             SCPI::isComponent);
}

void VfModuleComponentStorageFetchOnly::exportMetaData(QJsonObject &jsObj)
{
    QJsonObject jsonObj;
    jsonObj.insert("Description", m_description);
    jsObj.insert(m_componentName, jsonObj);
}

void VfModuleComponentStorageFetchOnly::exportSCPIInfo(QJsonArray &jsArr)
{
    if (m_scpiInfo)
        m_scpiInfo->appendSCPIInfo(jsArr);
}
