#ifndef RANGEMEASCHANNEL_H
#define RANGEMEASCHANNEL_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include <pcbinterface.h>

#include "basemeaschannel.h"
#include "rangeinfo.h"


enum rangemeaschannelCmds
{
    readresourcetypes,
    readresource,
    readresourceinfo,
    claimresource,
    readdspchannel,
    readchnalias,
    readrangelist,

    readrngalias,
    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readisavail,

    setrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection
};


enum replies
{
    ack,
    nack,
    error,
    debug,
    ident
};

const double sqrt2 = 1.41421356;

class cRangeMeasChannel:public cBaseMeasChannel
{
    Q_OBJECT

public:
    cRangeMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr);
    ~cRangeMeasChannel();
    quint32 setRange(QString range); // a statemachine gets started that returns cmdDone(quint32 cmdnr)
    quint32 readGainCorrection(double amplitude); // dito
    quint32 readPhaseCorrection(double frequency); // dito
    quint32 readOffsetCorrection(double amplitude); // dito
    double getGainCorrection(); // returns last read gain correction
    double getPhaseCorrection(); // returns last read phase correction
    double getOffsetCorrection(); // returns last read offset correction
    double getUrValue(QString range); // returns upper range value of range
    double getUrValue(); // returns upper range of actual range
    double getRejection(); // return nominal (100%) rejection of actual range
    bool isPossibleRange(QString range, double ampl); // test if range is possible with ampl
    bool isOverload(double ampl); // test if ampl is overload condition
    QString getOptRange(double ampl); // returns opt. range alias
    QString getMaxRange(); // returns alias of the range with max ur value

public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right

signals:
    void activationContinue(); // for internal loop control
    void cmdDone(quint32 cmdnr); // to signal we are ready
    void executionError(); // for error handling purpose (todo)

protected:
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, QVariant answer);

private:
    VeinEntity *m_pChannelEntity;
    VeinEntity *m_pChannelRangeListEntity;
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfo> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;

    // statemachine for activating a rangemeaschannel
    QStateMachine m_activationMachine;
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_claimResourceState; // we claim our resource
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readDspChannelState; // we query our dsp channel
    QState m_readChnAliasState; // we query our alias
    QState m_readRangelistState; // we query our range list
    QState m_readRangeProperties1State; // we build up a loop for querying all the ranges properties
    QState m_readRangeProperties2State; //
    QFinalState m_activationDoneState;

    // statemachine for querying a measchannels range properties
    QStateMachine m_rangeQueryMachine;
    QState m_readRngAliasState;
    QState m_readTypeState;
    QState m_readUrvalueState;
    QState m_readRejectionState;
    QState m_readOVRejectionState;
    QState m_readisAvailState;
    QFinalState m_rangeQueryDoneState;

    void setRangeListEntity();
    void setChannelEntity();

    qint32 m_RangeQueryIt;
    cRangeInfo ri;

private slots:
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
    void pcbConnection();
    void readDspChannel();
    void readChnAlias();
    void readRangelist();
    void readRangeProperties1();
    void readRangeProperties2();
    void activationDone();

    // the slots for querying a measchannels range properties
    void readRngAlias();
    void readType();
    void readUrvalue();
    void readRejection();
    void readOVRejection();
    void readisAvail();
    void rangeQueryDone();

};

#endif // RANGEMEASCHANNEL_H
