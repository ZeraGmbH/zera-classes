#ifndef REFERENCEADJUSTMENT_H
#define REFERENCEADJUSTMENT_H

#include "referencemoduleconfigdata.h"
#include "referencemeaschannel.h"
#include "abstractfactoryserviceinterfaces.h"
#include <moduleactivist.h>
#include <dspinterface.h>
#include <pcbinterface.h>

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

class cReferenceAdjustment: public cModuleActivist
{
    Q_OBJECT
public:
    cReferenceAdjustment(cReferenceModule* module, cReferenceModuleConfigData* confData);
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void ActionHandler(QVector<float> *actualValues); // entry after received actual values
signals:
    void adjustContinue();
    void repeatStateMachine();
private:
    cReferenceModule* m_pModule; // the module we live in
    cReferenceModuleConfigData* m_pConfigData;
    DspInterfacePtr m_dspInterface;
    Zera::ProxyClientPtr m_dspClient;
    Zera::PcbInterfacePtr m_pPCBInterface;
    Zera::ProxyClientPtr m_pPCBClient;
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
public slots:
    void catchChannelReply(quint32 msgnr);
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
};

}

#endif // REFERENCEADJUSTMENT_H
