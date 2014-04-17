#ifndef JUSTIFYNORM_H
#define JUSTIFYNORM_H

#include <QObject>
#include <QHash>
#include <QTimer>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "moduleacitvist.h"

namespace ADJUSTMENT
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
    getoffsetcore
};
}

class cRangeModule;
class cRangeMeasChannel;
class cDspMeasData;
class VeinPeer;


namespace Zera {
namespace Server {
    class cDSPInterface;
}
}


class cAdjustManagement: public cModuleActivist
{
    Q_OBJECT

public:
    cAdjustManagement(cRangeModule* module, VeinPeer* peer, Zera::Server::cDSPInterface* iface, QStringList chnlist, double interval);
    virtual ~cAdjustManagement();

public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void ActionHandler(QVector<float> *actualValues); // entry after received actual values

signals:
    void activationContinue();
    void repeatStateMachine();
    void finishStateMachine();

protected:
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

private:
    cRangeModule* m_pRangemodule; // the module we live in
    VeinPeer* m_pPeer;
    Zera::Server::cDSPInterface* m_pDSPIFace; // our interface to dsp
    QStringList m_ChannelNameList; // the list of channels (names) we work on
    double m_fAdjInterval;
    QList<cRangeMeasChannel*> m_ChannelList; // here the real channel list
    QVector<float> m_ActualValues;
    quint8 m_nChannelIt;
    QHash<quint32, int> m_MsgNrCmdList;
    QTimer m_AdjustTimer;
    bool m_bAdjust;

    // statemachine for reading dsp correction data
    QStateMachine m_readCorrectionDSPMachine;
    QState m_readGainCorrState, m_readPhaseCorrState, m_readOffsetCorrState;
    QFinalState m_readCorrDoneState;

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
    void readGainCorr();
    void readPhaseCorr();
    void readOffsetCorr();
    void readCorrDone();

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

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
    void catchChannelReply(quint32 msgnr);

    void adjustPrepare();

};

#endif // JUSTIFYNORM_H
