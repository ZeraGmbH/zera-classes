#include "secpowermodulefinder.h"

int SecPowerModuleFinder::findEntity(const QString &refName, const VeinStorage::AbstractDatabase *veinDb)
{
    const QList<VeinStorage::AbstractDatabase::EntityComponent> frequencyNameComponents = veinDb->findAllComponents("INF_FreqOuts");
    for (const auto &frequencyNameComponent : frequencyNameComponents) {
        const QStringList powerModuleOuts = frequencyNameComponent.component->getValue().toStringList();
        if (powerModuleOuts.contains(refName))
            return frequencyNameComponent.entityId;
    }
    qCritical("Entity not found for %s!", qPrintable(refName));
    return -1;
}

