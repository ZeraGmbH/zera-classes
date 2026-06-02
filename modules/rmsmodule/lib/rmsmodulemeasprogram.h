#ifndef RMSMODULEMEASPROGRAM_H
#define RMSMODULEMEASPROGRAM_H

#include "rmsmodule.h"
#include "rmsmoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilterwithoutsumfifo.h>
#include "actualvaluestartstophandler.h"
#include <QFinalState>

namespace RMSMODULE
{

class cRmsModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cRmsModuleMeasProgram(cRmsModule* module,
                          const std::shared_ptr<BaseModuleConfiguration> &configuration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void newIntegrationtime(QVariant ti);
    void newIntegrationPeriod(QVariant period);
private:
    cRmsModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void dataAcquisitionDSP();
    void dataReadDSP();

    cRmsModule* m_pModule = nullptr;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on

    VfModuleComponent *m_pMeasureSignal = nullptr;
    VfModuleParameter* m_pIntegrationParameter = nullptr;
    VfModuleMetaData* m_pRMSPNCountInfo = nullptr;
    VfModuleMetaData* m_pRMSPPCountInfo = nullptr;

    DspVarGroupClientInterface* m_pParameterDSP = nullptr;
    DspVarGroupClientInterface* m_pActualValuesDSP = nullptr;

    // statemachine for activating gets the following states
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_unloadStart;
    QFinalState m_unloadDSPDoneState;

    TaskTemplatePtr m_taskDataAcquisition;

    MovingWindowFilterWithoutSumFifo m_movingwindowFilter;
};

}
#endif // RMSMODULEMEASPROGRAM_H
