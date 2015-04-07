#ifndef SEC1MODULEMEASPROGRAM_H
#define SEC1MODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "basemeasprogram.h"
#include "secinputinfo.h"
#include "ecalcinfo.h"

namespace Zera
{
namespace Proxy
{
    class cProxyClient;
}
}

class VeinPeer;
class VeinEntity;

class cDspMeasData;
class cDspIFace;
class QStateMachine;
class QState;
class QFinalState;


namespace SEC1MODULE
{

enum sec1moduleCmds
{
    sendrmident,
    testsec1resource,
    setecresource,
    readresourcetypes,
    readresource,
    fetchecalcunits,
    readrefinputalias,
    readrefinputmuxchannel,
    readdutinputalias,
    readdutinputmuxchannel,
    setpcbrefconstantnotifier,
    setsecintnotifier,

    freeecalcunits,
    freeecresource,
    freeusermem,

    actualizeprogress,
    actualizestatus,

    setsync,
    setmeaspulses,
    setmastermux,
    setslavemux,
    setmastermeasmode,
    setslavemeasmode,
    enableinterrupt,
    startmeasurement,

    stopmeas,
    fetchrefconstant,

    readintregister,
    resetintregister,
    readmtcount

};

#define irqPCBNotifier 10
#define irqSECNotifier 11

class cBaseModule;
class cSec1Module;
class cSec1ModuleConfigData;


class cSec1ModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT

public:
    cSec1ModuleMeasProgram(cSec1Module* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cSec1ModuleConfigData& configData);
    virtual ~cSec1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration


public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop


protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cSec1Module* m_pModule; // the module we live in
    cSec1ModuleConfigData& m_ConfigData;

    Zera::Server::cSECInterface* m_pSECInterface;
    Zera::Server::cPCBInterface* m_pPCBInterface;

    // statemachine for activating gets the following states
    QState resourceManagerConnectState; // connect to resource manager
    QState m_IdentifyState; // identify to resource manager
    QState m_testSEC1ResourceState; // test for our configured error calculator units
    QState m_setECResourceState; // here we try to set 2 ecalcunits at rm
    QState m_readResourceTypesState; // read all resource types ...at the moment we set a list of predefined resource types
    QState m_readResourcesState; // init to read all resource information for each type
    QState m_readResourceState; // read for 1 type
    QState m_testSecInputsState; // here we test if all our configured inputs are present, we don't set them because we only get information from here
    QState m_ecalcServerConnectState; // connect to ecalculator server
    QState m_fetchECalcUnitsState; // we try to fetch 2 error calc units from sec server
    QState m_pcbServerConnectState; // connect to pcb server
    QState m_readREFInputsState; // init to read all ref input informations
    QState m_readREFInputAliasState; // read for 1 input
    QState m_readREFInputMuxchannelState; // read for 1 input
    QState m_readDUTInputsState; // init to read all ref input informations
    QState m_readDUTInputAliasState; // read for 1 input
    QState m_readDUTInputMuxchannelState; // read for 1 input
    QState m_setpcbREFConstantNotifierState; // we get notified on refconstant changes
    QState m_setsecINTNotifierState; // we get notified on sec interrupts
    QFinalState m_activationDoneState; // and then we have finished

    // statemachine for deactivating
    QState m_freeECalculatorState; // we give back our ecalcunits to sec server
    QState m_freeECResource; // and also give them back to the resource manager
    QFinalState m_deactivationDoneState;

    // statemachine for starting error measurement
    QStateMachine m_startMeasurementMachine;
    QState m_setsyncState; // our 2 ecalc units must be synchronized
    QState m_setMeaspulsesState; // we set the desired measpulses
    QState m_setMasterMuxState; // we set the input selectors
    QState m_setSlaveMuxState;
    QState m_setMasterMeasModeState; // and the meas modes
    QState m_setSlaveMeasModeState;
    QState m_enableInterruptState;
    QState m_startMeasurementState;
    QFinalState m_startMeasurementDoneState; // here we start it

    // statemachine for interrupthandling;
    QStateMachine m_InterrupthandlingStateMachine;
    QState m_readIntRegisterState;
    QState m_resetIntRegisterState;
    QState m_readMTCountactState;
    QFinalState m_setECResultState;

    Zera::Proxy::cProxyClient* m_pRMClient;
    Zera::Proxy::cProxyClient* m_pSECClient;
    Zera::Proxy::cProxyClient* m_pPCBClient;

    QStringList m_ResourceTypeList;
    QHash<QString,QString> m_ResourceHash; // resourcetype, resourcelist ; seperated
    QHash<QString,cSecInputInfo> mREFSecInputInfoHash; // we hold a list of all our input properties
    QHash<QString,cSecInputInfo> mDUTSecInputInfoHash; // systemname from configfile->alias, csecinputinfo
    QStringList m_REFAliasList; // we want to have an ordered list with input alias
    QStringList m_DUTAliasList;
    qint32 m_nIt;
    QList<QString> m_sItList; // for interation over x input hash
    cSecInputInfo siInfo;
    QString m_sIt;

    cECalcChannelInfo m_MasterEcalculator;
    cECalcChannelInfo m_SlaveEcalculator;

    VeinEntity* m_pDutinputEntity;
    VeinEntity* m_pDutinputListEntity;
    VeinEntity* m_pRefinputEntity;
    VeinEntity* m_pRefinputListEntity;
    VeinEntity* m_pRefConstantEntity;
    VeinEntity* m_pRefConstantLimitsEntity;
    VeinEntity* m_pDutConstantEntity;
    VeinEntity* m_pDutConstantLimitsEntity;
    VeinEntity* m_pMeasPulsesEntity;
    VeinEntity* m_pMeasPulsesLimitsEntity;
    VeinEntity* m_pTargetValueEntity;
    VeinEntity* m_pTargetValueLimitsEntity;
    VeinEntity* m_pStatusEntity;
    VeinEntity* m_pProgressEntity;
    VeinEntity* m_pResultEntity;
    VeinEntity* m_pStartStopEntity;

    QList<VeinEntity*> m_EntityList;
    void setInterfaceEntities();
    void handleChangedREFConst();
    void handleSECInterrupt();

    // vars dealing with error measurement
    bool m_brunning;
    QTimer m_ActualizeTimer; // after timed out we actualize progressvalue
    quint32 m_nStatus; // idle, started, running, finished
    double m_fResult; // error value in %
    quint32 m_nMTCNTact;
    quint32 m_nMTCNTfin;
    quint32 m_nTargetValue;
    double m_fProgress; // progress value in %
    quint32 m_nIntReg;

private slots:
    void resourceManagerConnect();
    void sendRMIdent();
    void testSEC1Resource();
    void setECResource();
    void readResourceTypes();
    void readResources();
    void readResource();
    void testSecInputs();
    void ecalcServerConnect();
    void fetchECalcUnits();
    void pcbServerConnect();
    void readREFInputs();
    void readREFInputAlias();
    void readREFInputMuxchannel();
    void readDUTInputs();
    void readDUTInputAlias();
    void readDUTInputMuxchannel();
    void setpcbREFConstantNotifier();
    void setsecINTNotifier();
    void activationDone();

    void freeECalculator();
    void freeECResource();
    void deactivationDone();

    void setSync();
    void setMeaspulses();
    void setMasterMux();
    void setSlaveMux();
    void setMasterMeasMode();
    void setSlaveMeasMode();
    void enableInterrupt();
    void startMeasurement();
    void startMeasurementDone();

    void readIntRegister();
    void resetIntRegister();
    void readMTCountact();
    void setECResult();

    void newStartStop(QVariant startstop);
    void Actualize();

};
}

#endif // SEC1MODULEMEASPROGRAM_H
