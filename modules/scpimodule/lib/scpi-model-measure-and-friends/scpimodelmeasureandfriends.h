#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "scpimodelbase.h"
#include "scpiinterface.h"
#include "scpidelegatemeasure.h"
#include "scpicmdinfo.h"
#include <QHash>

namespace SCPIMODULE
{

class cSCPIModule;

/* This class creates of SCPI tree from Vein INF_MODULEINTERFACE of:
 * * MEASURE/CONFIGURE/READ/INIT/FETCH models (VfModuleComponent added to m_pModule->m_veinComponentsWithMetaAndScpi)
 * * Parameters (VfModuleComponent::setScpiInfo / eSCPIEntryType::isComponent)
 * * CATALOGS (VfModuleComponent::setScpiInfo / eSCPIEntryType::isCatalog)
 */
class ScpiModelMeasureAndFriends : public ScpiModelBase
{
    Q_OBJECT
public:
    explicit ScpiModelMeasureAndFriends(cSCPIModule* module);
    ~ScpiModelMeasureAndFriends() override;

    bool setupScpi(cSCPIInterface *scpiInterface);

    QHash<QString, MeasureScpiNodeDelegatePtr>* getSCPIMeasDelegateHash();
    void updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue);

private:
    cSCPIModule* m_pModule = nullptr;
    QHash<QString, MeasureScpiNodeDelegatePtr> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd

    void addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);
    void addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                               const ScpiDelegateMeasure::Params &params,
                               const QJsonObject &veinComponentInfo = QJsonObject());
    QList<VeinComponentScpiMeasureSequence*> m_measureObjectsToDelete;
};

}

#endif // SCPIMODULEINTERFACE_H
