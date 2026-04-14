#ifndef DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H
#define DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H

#include <vs_abstractdatabase.h>
#include <QList>
#include <QJsonObject>

class DspSuperModuleIntegrationComponentFinder
{
public:
    struct Component {
        int entityId;
        QString componentName;
    };
    static QList<Component> findIntegrationTimeComponents(const VeinStorage::AbstractDatabase *storageDb);
    static QList<Component> findIntegrationPeriodComponents(const VeinStorage::AbstractDatabase *storageDb);
    static VeinStorage::AbstractComponentPtr componentToVein(const VeinStorage::AbstractDatabase *storageDb, const Component &component);

private:
    static QList<Component> findIntegrationComponents(const VeinStorage::AbstractDatabase *storageDb, const QString &integrationUnit);
    static QJsonObject getModuleInterface(const VeinStorage::AbstractDatabase *storageDb, int entityId);
    static QJsonObject getParInterval(const VeinStorage::AbstractDatabase *storageDb, int entityId);
};

#endif // DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H
