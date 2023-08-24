#ifndef THDNMODULEMEASPROGRAM_H
#define THDNMODULEMEASPROGRAM_H

#include "basedspmeasprogram.h"
#include "measchannelinfo.h"
#include "movingwindowfilter.h"
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>


class cDspMeasData;
class QStateMachine;
class QState;
class QFinalState;

class VfModuleComponent;
class VfModuleParameter;
class VfModuleMetaData;
class VfModuleActvalue;
class cMovingwindowFilter;


namespace THDNMODULE
{

enum thdnmoduleCmds
{
    resourcemanagerconnect,
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfos,
    readresourceinfo,
    pcbserverconnect,
    readsamplerate,
    readalias,
    readunit,
    readdspchannel,
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

#define irqNr 4

class cThdnModuleConfigData;
class cThdnModule;

class cThdnModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cThdnModuleMeasProgram(cThdnModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cThdnModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cThdnModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cThdnModule* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pThdnCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationTimeParameter;

    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QList<QString> channelInfoReadList; // a list of all channel info we have to read
    QString channelInfoRead; // the actual channel info we are working on
    quint32 m_nSRate; // number of samples / signal period

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_readResourceTypesState;
    QState m_readResourceState;
    QState m_readResourceInfosState;
    QState m_readResourceInfoState;
    QState m_readResourceInfoDoneState;
    QState m_pcbserverConnectState;
    QState m_readSampleRateState;
    QState m_readChannelInformationState;
    QState m_readChannelAliasState;
    QState m_readChannelUnitState;
    QState m_readDspChannelState;
    QState m_readDspChannelDoneState;

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

    void setActualValuesNames();
    void setSCPIMeasInfo();
    void setupDemoOperation();

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfos();
    void readResourceInfo();
    void readResourceInfoDone();

    void pcbserverConnect();
    void readSampleRate();
    void readChannelInformation();
    void readChannelAlias();
    void readChannelUnit();
    void readDspChannel();
    void readDspChannelDone();

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

};

}
#endif // THDNMODULEMEASPROGRAM_H
