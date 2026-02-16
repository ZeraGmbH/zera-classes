#ifndef PERIODAVERAGEMODULEMEASPROGRAM_H
#define PERIODAVERAGEMODULEMEASPROGRAM_H

#include "periodaveragemodule.h"
#include "periodaveragemoduleconfiguration.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <timerperiodicqt.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace PERIODAVERAGEMODULE
{

enum periodaveragemoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
};

class PeriodAverageModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    PeriodAverageModuleMeasProgram(PeriodAverageModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;

public slots:
    virtual void start() override;
    virtual void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    PeriodAverageModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();

    PeriodAverageModule* m_pModule;
    ChannelRangeObserver::SystemObserverPtr m_observer;
    ActualValueStartStopHandler m_startStopHandler;

    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_periodCountParameter;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating
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

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newPeriodCount(const QVariant &periodCount);
};

}
#endif // PERIODAVERAGEMODULEMEASPROGRAM_H
