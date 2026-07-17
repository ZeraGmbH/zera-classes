#ifndef SCPIMODULEINTERFACE_H
#define SCPIMODULEINTERFACE_H

#include "scpimodelbase.h"
#include "scpicmdinfo.h"
#include "scpiinterface.h"
#include "scpidelegatemeasure.h"
#include <QHash>

namespace SCPIMODULE
{

class cSCPIModule;

/* This class creates SCPI tree from Vein INF_MODULEINTERFACE of:
 * MEASURE/CONFIGURE/READ/INIT/FETCH models (VfModuleComponent added to m_pModule->m_veinComponentsWithMetaAndScpi)
 */
class ScpiModelMeasure : public ScpiModelBase
{
    Q_OBJECT
public:
    explicit ScpiModelMeasure(cSCPIModule* module);

    void setupScpi(cSCPIInterface *scpiInterface);

    void addClient(cSCPIClient *client);
    void removeClient(cSCPIClient *client);

private:
    typedef QHash<QString /* scpiPath */, ScpiDelegateMeasurePtr> VeinComponentByScpiPath;
    void addScpiCmdForVeinComponent(cSCPIInterface *scpiInterface,
                                    VeinComponentByScpiPath &scpiNodesAdded,
                                    const cSCPICmdInfoPtr &scpiComponentCmdInfo);
    void addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                               VeinComponentByScpiPath &scpiNodesAdded,
                               const ScpiDelegateMeasure::Params &params,
                               const VeinComponentId &componentId,
                               const QJsonObject &veinComponentInfo = QJsonObject());

    cSCPIModule* m_pModule = nullptr;
};

}

#endif // SCPIMODULEINTERFACE_H
