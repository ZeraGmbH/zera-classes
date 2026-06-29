#ifndef THDNSHADOWMODULEMEASPROGRAM_H
#define THDNSHADOWMODULEMEASPROGRAM_H

#include "basemeasprogram.h"
#include "thdnmodule.h"

namespace THDNMODULE
{

class cThdnShadowModuleMeasProgram : public cBaseMeasProgram
{
    Q_OBJECT
public:
    explicit cThdnShadowModuleMeasProgram(cThdnModule* module);
    void generateVeinInterface() override;

private slots:
    void start() override;
    void stop() override;
    void newIntegrationtime(const QVariant &ti);
private:
    cThdnModule* m_pModule = nullptr;
    QState m_activationDoneState;
    QState m_deactivationDoneState;

    QList<VeinStorage::AbstractComponentPtr> m_sourceActValueList;
    QList<VfModuleComponent*> m_veinActValueList;

    VeinStorage::AbstractComponentPtr m_sourceMeasSignal;
    VfModuleComponent* m_pMeasureSignal = nullptr;

    VeinStorage::AbstractComponentPtr m_sourceIntegrationTimeParameter;
    VfModuleParameter* m_pIntegrationTimeParameter = nullptr;

    bool m_started = false;
};

}
#endif // THDNSHADOWMODULEMEASPROGRAM_H
