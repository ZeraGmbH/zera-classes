#ifndef SAMPLECHANNEL_H
#define SAMPLECHANNEL_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include <pcbinterface.h>

#include "basesamplechannel.h"
#include "reply.h"


class cVeinModuleParameter;

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
class cSampleModuleConfigData;

class cSampleChannel: public cBaseSampleChannel
{
    Q_OBJECT

public:
    cSampleChannel(cSampleModule* module,Zera::Proxy::cProxy* proxy, cSampleModuleConfigData& configdata, quint8 chnnr);
    ~cSampleChannel();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready


protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cSampleModule* m_pModule;
    cSampleModuleConfigData& m_ConfigData;

    cVeinModuleParameter *m_pChannelRange;

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

    //QState m_readRangeProperties1State; // we build up a loop for querying all the ranges properties
    //QState m_readRangeProperties2State; //
    //QState m_readRangeProperties3State;

    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    // statemachine for freeing resources
    QStateMachine m_freeResourceStatemachine;
    QState m_freeResourceStartState;
    QFinalState m_freeResourceDoneState;

    void setRangeValidator();
    void setChannelNameMetaInfo();

    qint32 m_RangeQueryIt;

    Zera::Proxy::cProxyClient* m_pRMClient;
    Zera::Proxy::cProxyClient* m_pPCBClient;

private slots:
    void rmConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
    void pcbConnection();
    void readDspChannel();
    void readChnAlias();
    void readSampleRate();
    void readUnit();
    void readRangelist();
    void readRangeProperties1();
    void readRangeProperties3();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void newPLLRange(QVariant rng);
};

}

#endif // SAMPLEMEASCHANNEL_H
