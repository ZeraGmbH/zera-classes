#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "rangemodule.h"
#include "rangemoduleconfigdata.h"
#include <moduleactivist.h>
#include <dspinterface.h>
#include <networkconnectioninfo.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <timertemplateqt.h>

namespace RANGEMODULE
{

enum adjustmentCmds
{
    readgaincorr,
    readphasecorr,
    readoffsetcorr,

    writegaincorr,
    writephasecorr,
    writeoffsetcorr,

    getgaincorr,
    getphasecorr,
    getoffsetcore,

    subdcdsp,
};


class cAdjustManagement: public cModuleActivist
{
    Q_OBJECT
public:
    cAdjustManagement(cRangeModule* module, QStringList chnlist, QStringList subdclist, adjustConfPar *adjustmentConfig);
    void generateVeinInterface() override;
public slots:
    virtual void ActionHandler(QVector<float> *actualValues); // entry after received actual values
signals:
    void repeatStateMachine();
    void finishStateMachine();
private:
    double getIgnoreRmsCorrFactor();

    cRangeModule* m_pModule; // the module we live in
    QStringList m_ChannelNameList; // the list of channels (names) we work on
    QStringList m_subdcChannelNameList; // the list of channels we have to subtract dc
    adjustConfPar *m_adjustmentConfig;
    Zera::DspInterfacePtr m_dspInterface;
    Zera::ProxyClientPtr m_dspClient;
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pcbClient;
    QList<cRangeMeasChannel*> m_ChannelList; // here the real channel list
    QList<cRangeMeasChannel*> m_subDCChannelList;
    // Those are the actual values without preSacling
    quint8 m_nChannelIt;
    QHash<quint32, int> m_MsgNrCmdList;
    TimerTemplateQtPtr m_AdjustTimer;
    bool m_bAdjustTrigger;

    VfModuleComponent *m_pAdjustmentInfo;
    VfModuleParameter *m_ParIgnoreRmsValues;
    VfModuleParameter *m_ParIgnoreRmsValuesOnOff;
    QList<VfModuleParameter*> m_invertedPhasesParList;

    // statemachine for activating gets the following states
    QState m_pcbserverConnectState;
    QState m_dspserverConnectState;
    QState m_readGainCorrState;
    QState m_readPhaseCorrState;
    QState m_readOffsetCorrState;
    QState m_setSubDCState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;


    // statemachine for writing dsp correction data
    QStateMachine m_writeCorrectionDSPMachine;
    QState m_writeGainCorrState, m_writePhaseCorrState, m_writeOffsetCorrState;
    QFinalState m_writeCorrDoneState;

    // statemachine for adjustment
    QStateMachine m_adjustMachine;
    QState m_getGainCorrFromPcbServerState, m_prepareGainCorrForDspServerState;
    QState m_getPhaseCorrFromPcbServerState, m_preparePhaseCorrForDspServerState;
    QState m_getOffsetCorrFromPcbServerState, m_prepareOffsetCorrForDspServerState;
    QFinalState m_adjustDoneState;

    cDspMeasData* m_pGainCorrectionDSP; // copy of dsp internal correction data
    cDspMeasData* m_pPhaseCorrectionDSP;
    cDspMeasData* m_pOffsetCorrectionDSP;

    float* m_fGainCorr;
    QVector<float> m_fGainKeeperForFakingRmsValues;
    float* m_fPhaseCorr;
    float* m_fOffsetCorr;


private slots:
    void pcbserverConnect();
    void dspserverConnect();
    void readGainCorr();
    void readPhaseCorr();
    void readOffsetCorr();
    void setSubDC();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    void writeGainCorr();
    void writePhaseCorr();
    void writeOffsetCorr();

    void getGainCorrFromPcbServer();
    void prepareGainCorrForDspServer();
    void getPhaseCorrFromPcbServer();
    void preparePhaseCorrForDspServer();
    void getOffsetCorrFromPcbServer();
    void prepareOffsetCorrForDspServer();
    void getCorrDone();

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void catchChannelReply(quint32 msgnr);

    void adjustPrepare();

    void parIgnoreRmsValuesOnOffChanged(QVariant newValue);
    void parIgnoreRmsValuesChanged(QVariant newValue);

    void parInvertedPhaseStateChanged(QVariant newValue);

};

}

#endif // ADJUSTMENT_H
