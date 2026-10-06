#include "dspsupermoduleintegrationcomponentfinder.h"
#include <QJsonDocument>

static const char* integrationTimeComponentName = "PAR_Interval";

QList<DspSuperModuleIntegrationComponentFinder::Component> DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(
    const VeinStorage::AbstractDatabase *storageDb)
{
    return findIntegrationComponents(storageDb, "s");
}

QList<DspSuperModuleIntegrationComponentFinder::Component> DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(
    const VeinStorage::AbstractDatabase *storageDb)
{
    return findIntegrationComponents(storageDb, "period");
}

QList<DspSuperModuleIntegrationComponentFinder::Component> DspSuperModuleIntegrationComponentFinder::findIntegrationComponents(
    const VeinStorage::AbstractDatabase *storageDb, const QString &integrationUnit)
{
    QList<Component> ret;
    const QList<int> entityList = storageDb->getEntityList();
    for (int entityId : entityList) {
        QJsonObject parInterval = getComponentInfo(storageDb, entityId, integrationTimeComponentName);
        if (!parInterval.isEmpty() && parInterval["Unit"] == integrationUnit)
            ret.append({entityId, integrationTimeComponentName});
    }
    return ret;
}

VeinStorage::AbstractComponentPtr DspSuperModuleIntegrationComponentFinder::componentToVein(
    const VeinStorage::AbstractDatabase *storageDb, const Component &component)
{
    return storageDb->findComponent(component.entityId, component.componentName);
}

QJsonObject DspSuperModuleIntegrationComponentFinder::getModuleInterface(const VeinStorage::AbstractDatabase *storageDb, int entityId)
{
    if (storageDb->hasStoredValue(entityId, "INF_ModuleInterface")) {
        QString moduleInfo = storageDb->getStoredValue(entityId, "INF_ModuleInterface").toString();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(moduleInfo.toUtf8());
        if ( !jsonDoc.isNull() && jsonDoc.isObject() )
             return jsonDoc.object();
    }
    return QJsonObject();
}

QJsonObject DspSuperModuleIntegrationComponentFinder::getComponentInfo(const VeinStorage::AbstractDatabase *storageDb,
                                                                       int entityId, const QString &componentName)
{
    QJsonObject moduleInterface = getModuleInterface(storageDb, entityId);
    QJsonObject componentInfo = moduleInterface["ComponentInfo"].toObject();
    return componentInfo[componentName].toObject();
}
