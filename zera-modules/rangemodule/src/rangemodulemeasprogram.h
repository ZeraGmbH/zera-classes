#ifndef RANGEMODULEMEASPROGRAM_H
#define RANGEMODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "basemeasprogram.h"

class QCoreApplication;

namespace MEASPROGRAM
{

enum rangemoduleCmds
{
    sendrmident,
    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    freepgrmem,
    freeusermem
};
}

namespace Zera
{
namespace Proxy
{
    class cProxyClient;
}
}

class VeinPeer;
class VeinEntity;
class cBaseModule;
class cDspMeasData;
class cDspIFace;
class QStateMachine;
class QState;
class cRangeModule;
class QFinalState;


class cRangeModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT

public:
    cRangeModuleMeasProgram(cRangeModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, Zera::Server::cDSPInterface* iface, cSocket* rmsocket, QStringList chnlist, float interval);
    virtual ~cRangeModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration


public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
    virtual void syncRanging(QVariant sync); //

protected:
    virtual void setDspVarList(); // dsp related stuff
    virtual void deleteDspVarList();
    virtual void setDspCmdList();
    virtual void deleteDspCmdList();

protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    bool m_bRanging;
    bool m_bIgnore;
    cRangeModule* m_pModule; // the module we live in
    float m_fInterval;
    QList<VeinEntity*> m_EntityList;
    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_serverConnectState;
    QState m_IdentifyState;
    QState m_claimPGRMemState;
    QState m_claimUSERMemState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_deactivateDSPState;
    QState m_freePGRMemState;
    QState m_freeUSERMemState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    Zera::Proxy::cProxyClient* m_pRMClient;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void serverConnect();
    void sendRMIdent();
    void claimPGRMem();
    void claimUSERMem();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSP();
    void freePGRMem();
    void freeUSERMem();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

};

#endif // RANGEMODULEMEASPROGRAM_H
