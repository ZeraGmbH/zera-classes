#ifndef PLLMEASCHANNEL_H
#define PLLMEASCHANNEL_H

#include "rangeinfo.h"
#include <basemeaschannel.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace SAMPLEMODULE
{

enum pllmeaschannelCmds
{
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
    cPllMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                    NetworkConnectionInfo pcbsocket,
                    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                    QString moduleName);
    void generateVeinInterface() override;
    quint32 setyourself4PLL(QString samplesysname); // a statemachine gets started that returns cmdDone(quint32 cmdnr)
    quint32 setPLLMode(QString samplesysname, QString mode);
    double getUrValue(); // returns upper range of actual range
signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) override;
private:
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sActRange; // the actual range set (name)

    // statemachine for activating a rangemeaschannel
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
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

    qint32 m_RangeQueryIt;
    cRangeInfoWithConstantValues ri;

    Zera::ProxyClientPtr m_pcbClient;

private slots:
    void pcbConnection();
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
