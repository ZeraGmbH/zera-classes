#ifndef SAMPLECHANNEL_H
#define SAMPLECHANNEL_H

#include "samplemoduleconfigdata.h"
#include "moduleactivist.h"
#include <pcbinterface.h>
#include <vfmoduleparameter.h>
#include <QFinalState>

namespace SAMPLEMODULE
{
enum samplechannelCmds
{
    readchnaliassamplechannel,
    readrangelistsamplechannel,
    setsamplechannelrange
};

class cSampleModule;

class cSampleChannel: public cModuleActivist
{
    Q_OBJECT
public:
    cSampleChannel(cSampleModule* module, cSampleModuleConfigData& configdata);
    ~cSampleChannel();
    void generateVeinInterface() override;
signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    QString getName();
    QString getAlias();
    void setRangeValidator();

    QString m_sName; // the channel's system name
    cSampleModule* m_pModule;
    cSampleModuleConfigData& m_ConfigData;
    Zera::PcbInterfacePtr m_pPCBInterface;
    Zera::ProxyClientPtr m_pPCBClient;

    VfModuleParameter *m_pChannelRange;

    QStringList m_RangeNameList; // a list of all range's names
    QString m_sActRange; // the actual range set
    QString m_sNewRange;

    // the things we have to query from our pcb server
    QString m_sAlias; // the channel' alias
    QString m_sUnit; // we don't need but for api satisfaction ...
    QString m_sDescription; // we take description from resource manager
    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activating a samplechannel
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readChnAliasState; // we query our alias
    QState m_readRangelistState; // we query our range list

    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QFinalState m_deactivationDoneState;

    qint32 m_RangeQueryIt;

private slots:
    void pcbConnection();
    void readChnAlias();
    void readRangelist();
    void activationDone();

    void deactivationDone();

    void newPLLRange(QVariant rng);
};

}

#endif // SAMPLEMEASCHANNEL_H
