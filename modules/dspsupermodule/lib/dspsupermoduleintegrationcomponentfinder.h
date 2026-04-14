#ifndef DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H
#define DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H

#include <vs_abstractdatabase.h>
#include <QList>
#include <QJsonObject>

class DspSuperModuleIntegrationComponentFinder
{
public:
    struct Components {
        int entityId;
        QString componentName;
    };
    static QList<Components> findIntegrationTimeComponents(const VeinStorage::AbstractDatabase *storageDb);
    static QList<Components> findIntegrationPeriodComponents(const VeinStorage::AbstractDatabase *storageDb);

private:
    static QJsonObject getModuleInterface(const VeinStorage::AbstractDatabase *storageDb, int entityId);
    static QJsonObject getParInterval(const VeinStorage::AbstractDatabase *storageDb, int entityId);
};

#endif // DSPSUPERMODULEINTEGRATIONCOMPONENTFINDER_H
