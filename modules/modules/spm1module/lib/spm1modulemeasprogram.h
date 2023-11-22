#ifndef SPM1MODULEMEASPROGRAM_H
#define SPM1MODULEMEASPROGRAM_H

#include "spm1moduleconfigdata.h"
#include "secrefconstantobserver.h"
#include "secresourcetypelist.h"
#include <basemeasprogram.h>
#include <clientactivecomponent.h>
#include "secmeasinputdictionary.h"
#include <secinterface.h>
#include <timerperiodicqt.h>

namespace SPM1MODULE
{

enum spm1moduleCmds
{
    sendrmident,
    testsec1resource,
    setecresource,
    readresource,
    fetchecalcunits,
    readrefInputalias,
    setsecintnotifier,

    freeecalcunits,
    freeecresource,

    actualizestatus,
    actualizeenergy,
    actualizepower,

    setsync,
    setmeaspulses,
    setmastermux,
    setslavemux,
    setmastermeasmode,
    setslavemeasmode,
    enableinterrupt,
    startmeasurement,

    stopmeas,

    readintregister,
    resetintregister,
    readvicount,
    readtcount

};

class cSpm1Module;

class cSpm1ModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cSpm1ModuleMeasProgram(cSpm1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSpm1ModuleMeasProgram();
    virtual void generateInterface() override; // here we export our interface (entities)

public slots:
    void start() override; // difference between start and stop is that actual values
    virtual void stop() override; // in interface are not updated when stop

private slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void onRefConstantChanged(QString refInputName);
    void resourceManagerConnect();
    void sendRMIdent();
    void testSEC1Resource();
    void setECResource();
    void readResources();
    void readResource();
    void testSpmInputs();
    void ecalcServerConnect();
    void fetchECalcUnits();
    void pcbServerConnect();
    void readREFInputs();
    void readREFInputAlias();
    void readREFInputDone();
    void setpcbREFConstantNotifier();
    void setsecINTNotifier();
    void activationDone();

    void stopECCalculator();
    void freeECalculator();
    void freeECResource();
    void deactivationDone();

    void setSync();
    void setSync2();
    void setMeaspulses();
    void setMasterMux();
    void setSlaveMux();
    void setSlave2Mux();
    void setMasterMeasMode();
    void setSlaveMeasMode();
    void setSlave2MeasMode();
    void enableInterrupt();
    void startMeasurement();
    void startMeasurementDone();

    void readIntRegister();
    void resetIntRegister();
    void readVICountact();
    void readTCountact();
    void setEMResult();
    void setRating();

    void newStartStop(QVariant startstop);
    void newRefConstant(QVariant refconst);
    void newRefInput(QVariant refinput);
    void newTargeted(QVariant targeted);
    void newMeasTime(QVariant meastime);
    void newT0Input(QVariant t0input);
    void newT1Input(QVariant t1input);
    void newUnit(QVariant unit);
    void newUpperLimit(QVariant limit);
    void newLowerLimit(QVariant limit);

    void Actualize();
    void clientActivationChanged(bool bActive);
    void stopMeasurement(bool bAbort);
    bool found(QList<TRefInput> &list, QString searched);

private:
    cSpm1ModuleConfigData* getConfData();
    void setInterfaceComponents();
    void setValidators();
    void setUnits();

    QStringList getEnergyUnitValidator();
    QString getEnergyUnit();
    QStringList getPowerUnitValidator();
    QString getPowerUnit();
    QString getRefInputDisplayString(QString inputName);
    void actualizeRefConstant();
    quint32 getStatus();
    void setStatus(quint32 status);

    void handleSECInterrupt();
    void updateDemoMeasurementResults();

    cSpm1Module* m_pModule; // the module we live in
    Zera::cSECInterface* m_pSECInterface;
    Zera::PcbInterfacePtr m_pcbInterface;

    // statemachine for activating gets the following states
    QState resourceManagerConnectState; // connect to resource manager
    QState m_IdentifyState; // identify to resource manager
    QState m_testSEC1ResourceState; // test for our configured error calculator units
    QState m_setECResourceState; // here we try to set 3 ecalcunits at rm
    QState m_readResourcesState; // init to read all resource information for each type
    QState m_readResourceState; // read for 1 type

    QState m_testSpmInputsState; // here we test if all our configured Inputs are present, we don't set them because we only get information from here

    QState m_ecalcServerConnectState; // connect to ecalculator server
    QState m_fetchECalcUnitsState; // we try to fetch 3 error calc units from sec server
    QState m_pcbServerConnectState; // connect to pcb server

    QState m_readREFInputsState; // init to read all ref Input informations
    QState m_readREFInputAliasState; // read for 1 Input
    QState m_readREFInputDoneState;
    QState m_setpcbREFConstantNotifierState; // we get notified on refconstant changes
    QState m_setsecINTNotifierState; // we get notified on sec interrupts

    QFinalState m_activationDoneState; // and then we have finished

    // statemachine for deactivating
    QState m_stopECalculatorState; // we stop running measurement
    QState m_freeECalculatorState; // we give back our ecalcunits to sec server
    QState m_freeECResource; // and also give them back to the resource manager
    QFinalState m_deactivationDoneState;

    // statemachine for starting error measurement
    QStateMachine m_startMeasurementMachine;
    QState m_setsyncState; // our 2 ecalc units must be synchronized
    QState m_setsync2State;
    QState m_setMeaspulsesState; // we set the desired measpulses
    QState m_setMasterMuxState; // we set the Input selectors
    QState m_setSlaveMuxState;
    QState m_setSlave2MuxState;
    QState m_setMasterMeasModeState; // and the meas modes
    QState m_setSlaveMeasModeState;
    QState m_setSlave2MeasModeState;
    QState m_enableInterruptState;
    QState m_startMeasurementState;
    QFinalState m_startMeasurementDoneState; // here we start it

    // statemachine for interrupthandling;
    QStateMachine m_finalResultStateMachine;
    QState m_readIntRegisterState;
    QState m_resetIntRegisterState;
    QState m_readFinalEnergyCounterState;
    QState m_readFinalTimeCounterState;
    QFinalState m_setEMResultState;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClient* m_pSECClient;
    Zera::ProxyClient* m_pPCBClient;

    SecResourceTypeList m_resourceTypeList;
    QHash<QString,QString> m_ResourceHash; // resourcetype, resourcelist ; seperated
    SecMeasInputDictionary m_refInputDictionary;
    SecRefConstantObserver m_refConstantObserver;
    QHash<QString, double> mPowerUnitFactorHash;

    QStringList m_REFAliasList; // we want to have an ordered list with Input alias
    qint32 m_nIt;
    QList<QString> m_sItList; // for interation over x Input hash
    QString m_sIt;

    QString m_masterErrCalcName;
    QString m_slaveErrCalcName;
    QString m_slave2ErrCalcName;

    VfModuleParameter* m_pRefInputPar;
    VfModuleParameter* m_pRefConstantPar;
    VfModuleParameter* m_pTargetedPar;
    VfModuleParameter* m_pMeasTimePar;

    VfModuleParameter* m_pT0InputPar;
    VfModuleParameter* m_pT1InputPar;
    VfModuleParameter* m_pInputUnitPar;
    VfModuleParameter* m_pStartStopPar;

    VfModuleParameter* m_pStatusAct;
    VfModuleParameter* m_pTimeAct;
    VfModuleParameter* m_pEnergyAct;
    VfModuleParameter* m_pPowerAct;
    VfModuleParameter* m_pResultAct;
    VfModuleParameter* m_pRefFreqInput;
    VfModuleParameter* m_pUpperLimitPar;
    VfModuleParameter* m_pLowerLimitPar;
    VfModuleParameter* m_pRatingAct;

    VfModuleParameter* m_pClientNotifierPar;
    ClientActiveComponent m_ClientActiveNotifier;

    // vars dealing with error measurement
    TimerTemplateQtPtr m_ActualizeTimer; // after timed out we actualize progressvalue
    double m_fResult = 0.0; // error value in %
    ECALCRESULT::enResultTypes m_eRating = ECALCRESULT::RESULT_PASSED;
    quint32 m_nTimerCountStart = 0; // master time counts down
    quint32 m_nEnergyCounterActual = 0; // slave / energy counts up
    quint32 m_nEnergyCounterFinal = 0;
    double m_fTimeSecondsActual = 0.0; // slave2 time counts up
    double m_fTimeSecondsFinal = 0.0;
    double m_fEnergy = 0.0;
    double m_fPower = 0.0;
    quint32 m_nIntReg = 0;

    // Some decisions - we have enough of configration params around
    static constexpr quint32 m_nActualizeIntervallLowFreq = 1000;
    static constexpr quint32 m_nActualizeIntervallHighFreq = 50;
};
}

#endif // SPM1MODULEMEASPROGRAM_H
