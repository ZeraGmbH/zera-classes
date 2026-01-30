#include "vfmodulecomponentstoragefetchonly.h"
#include <scpi.h>

VfModuleComponentStorageFetchOnly::VfModuleComponentStorageFetchOnly(int entityId,
                                                                     const QString &componentName,
                                                                     const QString &description,
                                                                     VeinStorage::AbstractDatabase *storageDb) :
    m_componentName(componentName),
    m_description(description),
    m_storageComponent(storageDb->getFutureComponent(entityId, componentName))
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

void VfModuleComponentStorageFetchOnly::setStorageGetCustomizer(AbstractComponentGetCustomizerPtr getCustomizer)
{
    m_storageComponent->setGetValueCustomizer(getCustomizer);
}

const VeinStorage::AbstractComponentPtr VfModuleComponentStorageFetchOnly::getStorageComponent()
{
    return m_storageComponent;
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
