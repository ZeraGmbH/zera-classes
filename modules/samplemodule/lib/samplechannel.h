#ifndef SAMPLECHANNEL_H
#define SAMPLECHANNEL_H

#include "samplemoduleconfigdata.h"
#include <basesamplechannel.h>
#include <vfmoduleparameter.h>
#include <QFinalState>

namespace SAMPLEMODULE
{
enum samplechannelCmds
{
    sendsamplechannelrmident,
    readresourcetypessamplechannel,
    readresourcesamplechannel,
    readresourceinfosamplechannel,
    claimresource,
    freeresource,
    readchnaliassamplechannel,
    readrangelistsamplechannel,
    setsamplechannelrange
};

class cSampleModule;

class cSampleChannel: public cBaseSampleChannel
{
    Q_OBJECT
public:
    cSampleChannel(cSampleModule* module, cSampleModuleConfigData& configdata, quint8 chnnr);
    ~cSampleChannel();
    void generateVeinInterface() override;
signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) override;
private:
    cSampleModule* m_pModule;
    cSampleModuleConfigData& m_ConfigData;

    VfModuleParameter *m_pChannelRange;

    QStringList m_RangeNameList; // a list of all range's names
    QString m_sActRange; // the actual range set
    QString m_sNewRange;

    // statemachine for activating a samplechannel
    QState m_rmConnectState; // we must connect first to resource manager
    QState m_IdentifyState; // we must identify ourself at resource manager
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_claimResourceState; // we claim our resource
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readChnAliasState; // we query our alias
    QState m_readRangelistState; // we query our range list

    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    void setRangeValidator();
    void setChannelNameMetaInfo();

    qint32 m_RangeQueryIt;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClientPtr m_pPCBClient;

private slots:
    void rmConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
    void pcbConnection();
    void readChnAlias();
    void readRangelist();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void newPLLRange(QVariant rng);
};

}

#endif // SAMPLEMEASCHANNEL_H
