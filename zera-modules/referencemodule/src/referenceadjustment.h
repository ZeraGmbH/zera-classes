#ifndef REFERENCEADJUSTMENT_H
#define REFERENCEADJUSTMENT_H

#include <QObject>
#include <QHash>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "moduleactivist.h"


class cDspMeasData;
class VeinPeer;

namespace Zera {
namespace Proxy {
    class cProxy;
    class cProxyClient;
}
namespace  Server {
    class cDSPInterface;
    class cPCBInterface;
}
}

class cReferenceModuleConfigData;

namespace REFERENCEMODULE
{

enum adjustmentCmds
{
    setreferencemode,
    setacmode,
    readgaincorr,
    readoffset2corr,
    writeoffsetadjustment,
    setrange0V,
    setrange10V
};


class cReferenceModule;
class cReferenceMeasChannel;
class cReferenceModuleConfigData;

class cReferenceAdjustment: public cModuleActivist
{
    Q_OBJECT

public:
    cReferenceAdjustment(cReferenceModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cReferenceModuleConfigData* confData);
    virtual ~cReferenceAdjustment();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &);

public slots:
    virtual void ActionHandler(QVector<float> *actualValues); // entry after received actual values

signals:
    void adjustContinue();
    void repeatStateMachine();

private:
    cReferenceModule* m_pModule; // the module we live in
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    VeinPeer* m_pPeer;
    cReferenceModuleConfigData* m_pConfigData;
    Zera::Server::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::Proxy::cProxyClient *m_pDspClient;
    Zera::Server::cPCBInterface *m_pPCBInterface;
    Zera::Proxy::cProxyClient *m_pPCBClient;
    QList<cReferenceMeasChannel*> m_ChannelList;
    QVector<float> m_ActualValues;
    quint32 m_nIgnoreCount;
    bool m_bAdjustmentDone;

    qint32 m_nRangeSetPending;
    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activating gets the following states
    QState m_pcbserverConnectState;
    QState m_set0VRangeState;
    QState m_dspserverConnectState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    // statemachine for reference value adjustment
    QStateMachine m_referenceAdjustMachine;
    QState m_readGainCorrState; // we fetch actual gain correction values
    QState m_readOffset2CorrState; // and also actual used offset adjustment data
    QState m_writeOffsetAdjustmentState; // before we write offset correction to ensure proper adjustment
    QState m_set10VRangeState;
    QFinalState m_referenceAdjustDoneState;

    cDspMeasData* m_pGainCorrectionDSP; // copy of dsp internal correction data
    cDspMeasData* m_pOffset2CorrectionDSP; // copy of dsp internal correction data (set 2)
    float* m_fGainCorr;
    float* m_fOffset2Corr;

private slots:
    void pcbserverConnect();
    void set0VRange();
    void dspserverConnect();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void readGainCorr();
    void readOffset2Corr();
    void writeOffsetAdjustment();
    void set10VRange();
    void referenceAdjustDone();

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
    void catchChannelReply(quint32 msgnr);
};

}

#endif // REFERENCEADJUSTMENT_H
