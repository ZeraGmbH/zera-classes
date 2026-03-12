#ifndef MODEMODULEINIT_H
#define MODEMODULEINIT_H

#include "basedspmeasprogram.h"
#include "modemoduleconfigdata.h"
#include <pcbinterface.h>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace MODEMODULE {

class cModeModule;

class cModeModuleInit: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cModeModuleInit(cModeModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override {};
    void stop() override {};

private:
    cModeModuleConfigData* getConfData();
    cModeModule* m_pModule;

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pPCBClient;

    // statemachine for activating gets the following states
    QState m_pcbserverConnectionState; // we try to get a connection to our pcb server
    QState m_setModeState;
    QState m_dspserverConnectionState;
    QState m_writeGainCorrState, m_writeGainCorr2State;
    QState m_writePhaseCorrState,m_writePhaseCorr2State;
    QState m_writeOffsetCorrState, m_writeOffsetCorr2State;
    QState m_setSubDCState;
    QState m_setSamplingSystemState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QFinalState m_deactivationDoneState;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void pcbserverConnect();
    void setMode();
    void dspserverConnect();
    void writeGainCorr();
    void writeGainCorr2();
    void writePhaseCorr();
    void writePhaseCorr2();
    void writeOffsetCorr();
    void writeOffsetCorr2();
    void setSubDC();
    void setSamplingsytem();
    void activationDone();

    void deactivationDone();
};

}


#endif // MODEMODULEINIT_H
