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

bool SecPowerModuleFinder::testConfiguredRefInputs(const TSecCommonReferenceConfigs &refConfigs,
                                                   const VeinStorage::AbstractDatabase *veinDb)
{
    bool allFound = true;
    for (const TRefInput &input : refConfigs.m_refInpList) {
        if (findEntity(input.inputName, veinDb) < 0) {
            allFound = false;
            qWarning("Reference input %s not found!", qPrintable(input.inputName));
        }
    }
    return allFound;
}

