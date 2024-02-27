#ifndef ADJUSTMENT_H
#define ADJUSTMENT_H

#include "rangemodule.h"
#include <moduleactivist.h>
#include "abstractfactoryserviceinterfaces.h"
#include <socket.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <timertemplateqt.h>
#include <QFinalState>

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
    cAdjustManagement(cRangeModule* module, cSocket* dspsocket, cSocket* pcbsocket, QStringList chnlist, QStringList subdclist, double interval);
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void ActionHandler(QVector<float> *actualValues); // entry after received actual values
signals:
    void repeatStateMachine();
    void finishStateMachine();
private:
    cRangeModule* m_pModule; // the module we live in
    cSocket* m_pDSPSocket;
    cSocket* m_pPCBSocket;
    QStringList m_ChannelNameList; // the list of channels (names) we work on
    QStringList m_subdcChannelNameList; // the list of channels we have to subtract dc
    double m_fAdjInterval;
    DspInterfacePtr m_dspInterface;
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
    QState m_getGainCorr1State, m_getGainCorr2State;
    QState m_getPhaseCorr1State, m_getPhaseCorr2State;
    QState m_getOffsetCorr1State, m_getOffsetCorr2State;
    QFinalState m_adjustDoneState;

    cDspMeasData* m_pGainCorrectionDSP; // copy of dsp internal correction data
    cDspMeasData* m_pPhaseCorrectionDSP;
    cDspMeasData* m_pOffsetCorrectionDSP;

    float* m_fGainCorr;
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

    void getGainCorr1();
    void getGainCorr2();
    void getPhaseCorr1();
    void getPhaseCorr2();
    void getOffsetCorr1();
    void getOffsetCorr2();
    void getCorrDone();

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void catchChannelReply(quint32 msgnr);

    void adjustPrepare();

};

}

#endif // ADJUSTMENT_H
