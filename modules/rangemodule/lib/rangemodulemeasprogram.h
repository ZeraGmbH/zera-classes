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
    cRangeModuleMeasProgram(cRangeModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
    virtual void syncRanging(QVariant sync); //
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cRangeModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cRangeModule* m_pModule; // the module we live in
    bool m_bRanging;
    bool m_bIgnore;
    quint16 m_nSamples;
    QStringList m_ChannelList; // the list of actual values we work on

    VfModuleComponent *m_pMeasureSignal;
    QList<VfModuleActvalue*> m_veinActValueList;
    QList<VfModuleActvalue*> m_veinRmsValueList;
    TimerTemplateQtPtr m_dspWatchdogTimer;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_dspserverConnectState;
    QState m_claimPGRMemState;
    QState m_claimUSERMemState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_deactivateDSPState;
    QState m_freePGRMemState;
    QState m_freeUSERMemState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    Zera::ProxyClientPtr m_rmClient;
    QString m_lastDisplayedFreq;

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

    void deactivateDSP();
    void freePGRMem();
    void freeUSERMem();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void onDspWatchdogTimeout();
};

}

#endif // RANGEMODULEMEASPROGRAM_H
