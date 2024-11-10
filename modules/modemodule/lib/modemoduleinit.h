#ifndef MODEMODULEINIT_H
#define MODEMODULEINIT_H

#include "moduleactivist.h"
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace MODEMODULE {

namespace MODEMODINIT {

enum modemoduleinitCmds
{
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    claimresource,
    setmode,
    writegaincorr,
    writegaincorr2,
    writephasecorr,
    writephasecorr2,
    writeoffsetcorr,
    writeoffsetcorr2,
    subdcdsp,
    setsamplingsystem,
    freeresource
};

}

class cModeModule;
class cModeModuleConfigData;


class cModeModuleInit: public cModuleActivist
{
    Q_OBJECT

public:
    cModeModuleInit(cModeModule* module, cModeModuleConfigData& configData);
    virtual ~cModeModuleInit();
    void generateVeinInterface() override;

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cModeModule* m_pModule; // the module we live in
    cModeModuleConfigData& m_ConfigData;

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pPCBClient;
    Zera::cDSPInterface *m_pDSPInterface;
    Zera::ProxyClient *m_pDSPClient;
    Zera::cRMInterface m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;

    QString m_sDescription;
    quint16 m_nPort; // the port adress of pcb server

    cDspMeasData* m_pCorrectionDSP;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_claimResourceState;
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
    QState m_freeResourceState;
    QFinalState m_deactivationDoneState;

private slots:
    void resourceManagerConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
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

    void freeResource();
    void deactivationDone();
};

}


#endif // MODEMODULEINIT_H
