#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "scpimodelbase.h"
#include "scpiinterface.h"
#include "scpidelegatetemplate.h"
#include "measurescpinodedelegate.h"
#include "scpicmdinfo.h"
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE
{

class cSCPIModule;
class cSCPICatalogCmdDelegate;

/* This class creates of SCPI tree from Vein INF_MODULEINTERFACE of:
 * * MEASURE/CONFIGURE/READ/INIT/FETCH models (VfModuleComponent added to m_pModule->m_veinComponentsWithMetaAndScpi)
 * * Parameters (VfModuleComponent::setScpiInfo / eSCPIEntryType::isComponent)
 * * CATALOGS (VfModuleComponent::setScpiInfo / eSCPIEntryType::isCatalog)
 */
class ScpiModelMeasureAndFriends : public ScpiModelBase
{
    Q_OBJECT
public:
    ScpiModelMeasureAndFriends(cSCPIModule* module);
    ~ScpiModelMeasureAndFriends() override;

    bool setupScpi(cSCPIInterface *scpiInterface);
    virtual void actualizeInterface(QVariant modInterface);

    QHash<QString, MeasureScpiNodeDelegatePtr>* getSCPIMeasDelegateHash();
    void updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue);

private:
    cSCPIModule* m_pModule = nullptr;
    QHash<QString, MeasureScpiNodeDelegatePtr> m_scpiMeasureDelegateHash; // a hash for measure cmd's ... needed for clean up and search for existing cmd
    QHash<QString, cSCPICatalogCmdDelegate*> m_scpiPropertyDelegateHash; // a hash with property delegates taht might need actualization when something changes

    void addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);
    void addRPCCommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo);
    void addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                               const QString &cmdparent,
                               const QString &cmd,
                               quint8 scpiCmdQueryFlags,
                               ScpiModelTypes modelType,
                               VeinComponentScpiMeasureSequence* measureObject,
                               QJsonObject veinComponentInfo = QJsonObject());
    void setXmlComponentInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    void setXmlComponentValidatorInfo(ScpiBaseDelegatePtr delegate, const QJsonObject &componentInfo);
    QJsonArray getValidatorEntries(QJsonObject validator);

    QList<VeinComponentScpiMeasureSequence*> m_measureObjectsToDelete;
};

}

#endif // SCPIMODULEINTERFACE_H
