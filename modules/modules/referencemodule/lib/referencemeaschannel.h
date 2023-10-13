#ifndef REFERENCEMEASCHANNEL_H
#define REFERENCEMEASCHANNEL_H

#include <rangeinfo.h>
#include <QHash>
#include <QStringList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include "basemeaschannel.h"

namespace REFERENCEMODULE
{

enum rangemeaschannelCmds
{
    sendmeaschannelrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    freeresource,
    readdspchannel,
    readchnalias,
    readsamplerate,
    readunit,
    readrangelist,

    readrngalias,
    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readisavail,

    setmeaschannelrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection,
    readmeaschannelstatus,
    resetmeaschannelstatus
};

class cReferenceMeasChannel:public cBaseMeasChannel
{
    Q_OBJECT

public:
    cReferenceMeasChannel(cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr, bool demo);
    ~cReferenceMeasChannel();
    virtual void generateInterface(); // here we export our interface (entities)

    quint32 setRange(QString range); // a statemachine gets started that returns cmdDone(quint32 cmdnr)

    double getUrValue();
    double getRejection(); // return nominal (100%) rejection of actual range

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    void setupDemoOperation();

    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfo> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_nStatus;
    bool m_demo;

    // statemachine for activating a referencemeaschannel
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
    cRangeInfo ri;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClient* m_pPCBClient;

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
};

}

#endif // REFERENCEMEASCHANNEL_H
