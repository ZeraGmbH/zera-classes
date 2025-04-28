#ifndef OSCIMODULEMEASPROGRAM_H
#define OSCIMODULEMEASPROGRAM_H

#include "actualvaluestartstophandler.h"
#include "oscimoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <QFinalState>
#include <timerperiodicqt.h>

namespace OSCIMODULE
{

enum oscimoduleCmds
{
    sendrmident,
    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
    freepgrmem,
    freeusermem
};

#define irqNr 6

class cOsciModule;

class cOsciModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cOsciModuleMeasProgram(cOsciModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cOsciModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cOsciModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pOsciCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pRefChannelParameter;

    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_pcbserverConnectState;
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

    void setActualValuesNames();

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();
    void pcbserverConnect();
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

    void newRefChannel(QVariant chn);

};

}
#endif // OSCIMODULEMEASPROGRAM_H
