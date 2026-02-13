#ifndef RMSMODULEMEASPROGRAM_H
#define RMSMODULEMEASPROGRAM_H

#include "rmsmoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilterwithoutsumfifo.h>
#include <timerperiodicqt.h>
#include "actualvaluestartstophandler.h"
#include <QFinalState>

namespace RMSMODULE
{

enum rmsmoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
};

class cRmsModule;

class cRmsModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cRmsModuleMeasProgram(cRmsModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
private:
    cRmsModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void setActualValuesNames();
    void setSCPIMeasInfo();

    cRmsModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on

    VfModuleComponent *m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationParameter;
    VfModuleMetaData* m_pRMSPNCountInfo;
    VfModuleMetaData* m_pRMSPPCountInfo;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_unloadStart;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    MovingWindowFilterWithoutSumFifo m_movingwindowFilter;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newIntegrationtime(QVariant ti);
    void newIntegrationPeriod(QVariant period);
};

}
#endif // RMSMODULEMEASPROGRAM_H
