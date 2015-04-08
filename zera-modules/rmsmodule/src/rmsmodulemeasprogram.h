#ifndef RMSMODULEMEASPROGRAM_H
#define RMSMODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "basedspmeasprogram.h"
#include "measchannelinfo.h"

namespace Zera {
namespace Proxy {
    class cProxy;
}
}

class cDspMeasData;
class VeinPeer;
class VeinEntity;
class QStateMachine;
class QState;
class QFinalState;

class cModuleSignal;
class cModuleParameter;
class cModuleInfo;
class cMovingwindowFilter;

namespace RMSMODULE
{

enum rmsmoduleCmds
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


#define irqNr 2

class cRmsModuleConfigData;
class cRmsModule;

class cRmsModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT

public:
    cRmsModuleMeasProgram(cRmsModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cRmsModuleConfigData& configdata);
    virtual ~cRmsModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &jsArr);

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
    cRmsModule* m_pModule;
    cRmsModuleConfigData& m_ConfigData;
    QStringList m_ActValueList; // the list of actual values we work on
    QList<VeinEntity*> m_EntityNamePNList; // we have a list for all rms names for phase neutral
    QList<VeinEntity*> m_EntityNamePPList; // and a list for all rms names for phase phase
    QList<VeinEntity*> m_EntityActValuePNList; // we have a list for all rms values for phase neutral
    QList<VeinEntity*> m_EntityActValuePPList; // and a list for all rms values for phase phase

    QList<VeinEntity*> m_EntityNameList; // we use this list for access
    QList<VeinEntity*> m_EntityActValueList; // also

    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QList<QString> channelInfoReadList; // a list of all channel info we have to read
    QString channelInfoRead; // the actual channel info we are working on
    quint32 m_nSRate; // number of samples / signal period
    cModuleSignal* m_pMeasureSignal;
    cModuleParameter* m_pIntegrationTimeParameter;
    cModuleInfo* m_pIntegrationTimeLimits;
    cModuleParameter* m_pIntegrationPeriodParameter;
    cModuleInfo* m_pIntegrationPeriodLimits;
    cModuleInfo* m_pRMSPNCountInfo;
    cModuleInfo* m_pRMSPPCountInfo;
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
    cMovingwindowFilter* m_pMovingwindowFilter;

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
    void newIntegrationPeriod(QVariant period);

};

}
#endif // RMSMODULEMEASPROGRAM_H
