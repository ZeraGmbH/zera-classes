#ifndef PERIODAVERAGEMODULEMEASPROGRAM_H
#define PERIODAVERAGEMODULEMEASPROGRAM_H

#include "periodaveragemodule.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <timerperiodicqt.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace PERIODAVERAGEMODULE
{

class PeriodAverageModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    explicit PeriodAverageModuleMeasProgram(PeriodAverageModule* module);
    void generateVeinInterface() override;

public slots:
    virtual void start() override;
    virtual void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void newPeriodCount(const QVariant &periodCount);
private:
    void setDspVarList();
    void setDspCmdList();
    void dataAcquisitionDSP();
    void dataReadDSP();

    PeriodAverageModule* m_pModule = nullptr;
    ChannelRangeObserver::SystemObserverPtr m_observer;
    ActualValueStartStopHandler m_startStopHandler;

    QList<VfModuleComponent*> m_periodValues;
    QList<VfModuleComponent*> m_averageValues;
    VfModuleComponent* m_pMeasureSignal = nullptr;
    VfModuleParameter* m_periodCountParameter= nullptr;

    DspVarGroupClientInterface* m_pParameterDSP= nullptr;
    DspVarGroupClientInterface* m_pActualValuesDSP= nullptr;

    // statemachine for activating
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_unloadStart;
    QFinalState m_unloadDSPDoneState;

    TaskTemplatePtr m_taskDataAcquisition;
};

}
#endif // PERIODAVERAGEMODULEMEASPROGRAM_H
