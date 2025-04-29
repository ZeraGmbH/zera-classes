#ifndef RANGEMODULEMEASPROGRAM_H
#define RANGEMODULEMEASPROGRAM_H

#include "rangemoduleconfigdata.h"
#include <basemeasmodule.h>
#include <basedspmeasprogram.h>
#include <proxyclient.h>
#include <QList>
#include <QStateMachine>
#include <QFinalState>
#include <timerperiodicqt.h>
#include <logstatisticsasyncfloat.h>

namespace RANGEMODULE
{

enum rangemoduleCmds
{
    sendrmident,
    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    freepgrmem,
    freeusermem
};

class cRangeModule;

class cRangeModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cRangeModuleMeasProgram(cRangeModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
    virtual void syncRanging(QVariant sync); //
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cRangeModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cRangeModule* m_pModule; // the module we live in
    bool m_bRanging;
    bool m_bIgnore;

    VfModuleComponent *m_pMeasureSignal;
    QList<VfModuleComponent*> m_veinActValueList;
    QList<VfModuleComponent*> m_veinRmsValueList;
    TimerTemplateQtPtr m_dspWatchdogTimer;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_dspserverConnectState;
    QState m_claimPGRMemState;
    QState m_claimUSERMemState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_freePGRMemState;
    QState m_freeUSERMemState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    Zera::ProxyClientPtr m_rmClient;
    LogStatisticsAsyncFloat m_frequencyLogStatistics;

    void setActualValuesNames();
    void setSCPIMeasInfo();
    void restartDspWachdog();

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();
    void dspserverConnect();
    void claimPGRMem();
    void claimUSERMem();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void freePGRMem();
    void freeUSERMem();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void onDspWatchdogTimeout();
};

}

#endif // RANGEMODULEMEASPROGRAM_H
