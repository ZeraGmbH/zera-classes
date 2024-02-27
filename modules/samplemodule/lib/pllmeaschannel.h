#ifndef PLLMEASCHANNEL_H
#define PLLMEASCHANNEL_H

#include "rangeinfo.h"
#include <basemeaschannel.h>
#include <rminterface.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace SAMPLEMODULE
{

enum pllmeaschannelCmds
{
    sendpllchannelrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    readdspchannel,
    readchnalias,
    readunit,
    readsamplerate,
    readrangelist,
    readrange,
    setchannelrangenotifier,
    set4PLL,

    readrngalias,
    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readisavail
};

#define notifierNr 1

// we have to hold object of all pll meas channels that can be used for pll selection
// because they provide necessary information for measuring program as well as pll observation

class cPllMeasChannel:public cBaseMeasChannel
{
    Q_OBJECT
public:
    cPllMeasChannel(cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr);
    virtual void generateInterface(); // here we export our interface (entities)
    quint32 setyourself4PLL(QString samplesysname); // a statemachine gets started that returns cmdDone(quint32 cmdnr)
    quint32 setPLLMode(QString samplesysname, QString mode);
    double getUrValue(); // returns upper range of actual range
signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sActRange; // the actual range set (name)

    // statemachine for activating a rangemeaschannel
    QState m_rmConnectState; // we must connect first to resource manager
    QState m_IdentifyState; // we must identify ourself at resource manager
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readDspChannelState; // we query our dsp channel
    QState m_readChnAliasState; // we query our alias
    QState m_readSampleRateState; // we read the sample nr
    QState m_readUnitState; // we read the meas channel unit volt ampere ...
    QState m_readRangelistState; // we query our range list
    QState m_readRangeProperties1State; // we build up a loop for querying all the ranges properties
    QState m_readRangeProperties2State; //
    QState m_readRangeProperties3State;
    QState m_setSenseChannelRangeNotifierState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;


    // statemachine for querying a measchannels range properties
    QStateMachine m_rangeQueryMachine;
    QState m_readRngAliasState;
    QState m_readTypeState;
    QState m_readUrvalueState;
    QState m_readRejectionState;
    QState m_readOVRejectionState;
    QState m_readisAvailState;
    QFinalState m_rangeQueryDoneState;

    // statemachine for querying a measchannels range properties
    QStateMachine m_freeResourceStatemachine;
    QState m_freeResourceStartState;
    QFinalState m_freeResourceDoneState;

    qint32 m_RangeQueryIt;
    cRangeInfoWithConstantValues ri;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClientPtr m_pcbClient;

private slots:
    void rmConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void pcbConnection();
    void readDspChannel();
    void readChnAlias();
    void readSampleRate();
    void readUnit();
    void readRangelist();
    void readRangeProperties1();
    void readRangeProperties3();
    void setSenseChannelRangeNotifier();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    // the slots for querying a measchannels range properties
    void readRngAlias();
    void readType();
    void readUrvalue();
    void readRejection();
    void readOVRejection();
    void readisAvail();
    void rangeQueryDone();

    void readRange();
};

}


#endif // PLLMEASCHANNEL_H
