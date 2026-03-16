#ifndef DSPSUPERMODULEMEASPROGRAM_H
#define DSPSUPERMODULEMEASPROGRAM_H

#include "dspsupermodule.h"
#include "dspsupermoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace DSPSUPERMODULE
{

class DspSuperModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    DspSuperModuleMeasProgram(DspSuperModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;

public slots:
    virtual void start() override {};
    virtual void stop() override {};
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private slots:
    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void sendSuperModuleCmd();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();
private:
    DspSuperModuleConfigData* getConfData();
    void setInterfaceActualValues(const QVector<float> &actualValues);
    void setDspVarList();
    void setDspCmdList();
    void dataAcquisitionDSP();
    void dataReadDSP();

    DspSuperModule* m_pModule = nullptr;

    DspVarGroupClientInterface* m_pActualValuesDSP = nullptr;

    // statemachine for activating
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_cmdMakeDspSuperModule;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_unloadStart;
    QFinalState m_unloadDSPDoneState;

    TaskTemplatePtr m_taskDataAcquisition;

};

}
#endif // DSPSUPERMODULEMEASPROGRAM_H
