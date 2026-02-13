#ifndef REFERENCEMODULEMEASPROGRAM_H
#define REFERENCEMODULEMEASPROGRAM_H

#include "referencemoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <timerperiodicqt.h>

namespace REFERENCEMODULE
{

enum referencemoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
};

#define irqNr 9

class cReferenceModule;

class cReferenceModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cReferenceModuleMeasProgram(cReferenceModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
private:
    cReferenceModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();

    cReferenceModule* m_pModule; // the module we live in
    QStringList m_ChannelList; // the list of actual values we work on
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
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    TimerTemplateQtPtr m_demoPeriodicTimer;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void handleDemoActualValues();
};

}

#endif // REFERENCEMODULEMEASPROGRAM_H
