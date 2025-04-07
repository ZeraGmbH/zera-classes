#ifndef FFTMODULEMEASPROGRAM_H
#define FFTMODULEMEASPROGRAM_H

#include "fftmodule.h"
#include "fftmoduleconfigdata.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilter.h>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <timerperiodicqt.h>

namespace FFTMODULE
{

enum fftmoduleCmds
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

class cFftModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cFftModuleMeasProgram(cFftModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    virtual void start() override;
    virtual void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cFftModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();
    quint16 calcFftResultLenHalf(quint8 fftOrder);

    cFftModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleComponent*> m_DCValueList;
    VfModuleMetaData* m_pFFTCountInfo;
    VfModuleMetaData* m_pFFTOrderInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pRefChannelParameter;
    VfModuleParameter* m_pIntegrationTimeParameter;
    quint16 m_nfftLen;

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
    QState m_deactivateDSPState;
    QState m_freePGRMemState;
    QState m_freeUSERMemState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    void setSCPIMeasInfo();
    void setActualValuesNames();

    cMovingwindowFilter m_movingwindowFilter;
    QVector<float> m_FFTModuleActualValues;

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

    void deactivateDSP();
    void freePGRMem();
    void freeUSERMem();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newIntegrationtime(QVariant ti);
    void newRefChannel(QVariant chn);
};

}
#endif // FFTMODULEMEASPROGRAM_H
