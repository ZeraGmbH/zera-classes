#ifndef SAMPLEMODULEMEASPROGRAM_H
#define SAMPLEMODULEMEASPROGRAM_H

#include "samplemoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace SAMPLEMODULE
{

enum samplemoduleCmds
{
    sendrmidentsample,
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

#define irqNr 8

class cSampleModule;

class cSampleModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cSampleModuleMeasProgram(cSampleModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSampleModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
protected:
    virtual void setDspVarList(); // dsp related stuff
    virtual void deleteDspVarList();
    virtual void setDspCmdList();
    virtual void deleteDspCmdList();
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cSampleModuleConfigData* getConfData();

    cSampleModule* m_pModule; // the module we live in
    quint16 m_nSamples;
    QStringList m_ChannelList; // the list of actual values we work on
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

    Zera::Proxy::ProxyClientPtr m_rmClient;

private slots:
    void setInterfaceActualValues(QVector<float>);

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
};

}

#endif // SAMPLEMODULEMEASPROGRAM_H
