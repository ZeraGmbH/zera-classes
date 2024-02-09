#ifndef SEC1MODULEMEASPROGRAM_H
#define SEC1MODULEMEASPROGRAM_H

#include "sec1module.h"
#include "sec1moduleconfigdata.h"
#include "clientactivecomponent.h"
#include "multipleresulthelper.h"
#include "secmeasinputdictionary.h"
#include "secresourcetypelist.h"
#include "secrefconstantobserver.h"
#include <doublevalidator.h>
#include <stringvalidator.h>
#include <basemeasprogram.h>
#include <QDateTime>
#include <timerperiodicqt.h>
#include <QElapsedTimer>

namespace SEC1MODULE
{

enum sec1moduleCmds
{
    sendrmident,
    testsec1resource,
    setecresource,
    readresource,
    fetchecalcunits,
    readrefInputalias,
    readdutInputalias,
    setsecintnotifier,

    freeecalcunits,
    freeecresource,

    actualizeprogress,
    actualizestatus,
    actualizeenergy,

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
    readvicount

};

class cSec1ModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cSec1ModuleMeasProgram(cSec1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSec1ModuleMeasProgram();
    virtual void generateInterface() override; // here we export our interface (entities)
public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override; // in interface are not updated when stop
private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void onRefConstantChanged(QString refInputName);
    void resourceManagerConnect();
    void sendRMIdent();
    void testSEC1Resource();
    void setECResource();
    void readResources();
    void readResource();
    void testSecInputs();
    void ecalcServerConnect();
    void fetchECalcUnits();
    void pcbServerConnect();
    void readREFInputs();
    void readREFInputAlias();
    void readREFInputDone();
    void readDUTInputs();
    void readDUTInputAlias();
    void readDUTInputDone();
    void setpcbREFConstantNotifier();
    void setsecINTNotifier();
    void activationDone();

    void stopECCalculator();
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
    void setRating();
    void setECResultAndResetInt();
    void checkForRestart();

    // vein change handlers
    void newStartStop(QVariant startstop);
    void newDutConstant(QVariant dutconst);
    void newDutConstantAuto(QVariant dutConstAuto);
    void newDutConstantScale(QVariant uINumDenomValue, const QString componentName);
    void newDutConstantUnit(QVariant dutconstunit);
    void newRefConstant(QVariant refconst);
    void newDutInput(QVariant dutinput);
    void newRefInput(QVariant refinput);
    void newMRate(QVariant mrate);
    void newTarget(QVariant target);
    void newEnergy(QVariant energy);
    void newUpperLimit(QVariant limit);
    void newLowerLimit(QVariant limit);

    void Actualize();
    void startNext();
    void clientActivationChanged(bool bActive);
    bool found(QList<QString>& list, QString searched);
    bool found(QList<TRefInput> &list, QString searched);

private:
    cSec1ModuleConfigData* getConfData();
    void setInterfaceComponents();
    void setValidators();

    QStringList getDutConstUnitValidator();
    QString getEnergyUnit();
    void initDutConstantUnit(QStringList sl);
    void initDutConstantUnit();

    void handleSECInterrupt();
    void cmpDependencies();
    void stopMeasurement(bool bAbort);

    const QString multiResultToJson();
    void multiResultToVein();
    double getUnitFactor();
    QString getRefInputDisplayString(QString inputName);
    void actualizeRefConstant();
    quint32 getStatus();
    void setStatus(quint32 status);

    double calculateDutConstant();
    void updateDemoMeasurementResults();
    void updateProgress(quint32 dUTPulseCounterActual);
    void deduceMeasStartTime(quint32 dUTPulseCounterActual);
    void setDateTimeNow(QDateTime &var, VfModuleParameter* veinParam);
    void setDateTime(QDateTime var, VfModuleParameter* veinParam);

    cSec1Module* m_pModule; // the module we live in
    Zera::cSECInterface* m_pSECInterface;

    // statemachine for activating gets the following states
    QState resourceManagerConnectState; // connect to resource manager
    QState m_IdentifyState; // identify to resource manager
    QState m_testSEC1ResourceState; // test for our configured error calculator units
    QState m_setECResourceState; // here we try to set 2 ecalcunits at rm
    QState m_readResourcesState; // init to read all resource information for each type
    QState m_readResourceState; // read for 1 type

    QState m_testSecInputsState; // here we test if all our configured Inputs are present, we don't set them because we only get information from here

    QState m_ecalcServerConnectState; // connect to ecalculator server
    QState m_fetchECalcUnitsState; // we try to fetch 2 error calc units from sec server
    QState m_pcbServerConnectState; // connect to pcb server

    QState m_readREFInputsState; // init to read all ref Input informations
    QState m_readREFInputAliasState; // read for 1 Input
    QState m_readREFInputDoneState;

    QState m_readDUTInputsState; // init to read all ref Input informations
    QState m_readDUTInputAliasState; // read for 1 Input
    QState m_readDUTInputDoneState;

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
    QState m_setMeaspulsesState; // we set the desired measpulses
    QState m_setMasterMuxState; // we set the Input selectors
    QState m_setSlaveMuxState;
    QState m_setMasterMeasModeState; // and the meas modes
    QState m_setSlaveMeasModeState;
    QState m_enableInterruptState;
    QState m_startMeasurementState;
    QFinalState m_startMeasurementDoneState; // here we start it

    // statemachine for interrupthandling;
    QStateMachine m_InterrupthandlingStateMachine;
    QState m_readIntRegisterState;
    QState m_readMTCountactState;
    QState m_calcResultAndResetIntState;
    QFinalState m_FinalState;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClient* m_pSECClient;
    Zera::ProxyClient* m_pPCBClient;

    SecResourceTypeList m_resourceTypeList;
    QHash<QString,QString> m_ResourceHash; // resourcetype, resourcelist ; seperated
    SecMeasInputDictionary m_refInputDictionary;
    SecMeasInputDictionary m_dutInputDictionary;
    SecRefConstantObserver m_refConstantObserver;

    QStringList m_REFAliasList; // we want to have an ordered list with Input alias
    QStringList m_DUTAliasList;
    qint32 m_nIt;
    QList<QString> m_sItList; // for interation over x Input hash
    QString m_sIt;

    QString m_masterErrCalcName;
    QString m_slaveErrCalcName;

    VfModuleParameter* m_pDutInputPar;
    VfModuleParameter* m_pRefInputPar;
    VfModuleParameter* m_pRefConstantPar;
    VfModuleParameter* m_pDutConstantPar;
    VfModuleParameter* m_pDutConstantAuto;
    VfModuleParameter* m_pDutConstantUScaleDenom;
    VfModuleParameter* m_pDutConstantUScaleNum;
    VfModuleParameter* m_pDutConstantIScaleDenom;
    VfModuleParameter* m_pDutTypeMeasurePoint;
    VfModuleParameter* m_pDutConstantIScaleNum;
    VfModuleParameter* m_pDutConstantUnitPar;
    VfModuleParameter* m_pMRatePar;
    VfModuleParameter* m_pTargetPar;
    VfModuleParameter* m_pEnergyPar;
    VfModuleParameter* m_pStartStopPar;
    VfModuleParameter* m_pStatusAct;
    VfModuleParameter* m_pProgressAct;
    VfModuleParameter* m_pEnergyAct;
    VfModuleParameter* m_pEnergyFinalAct;
    VfModuleParameter* m_pResultAct;
    VfModuleParameter* m_pRefFreqInput;
    VfModuleParameter* m_pContinuousPar;
    VfModuleParameter* m_pUpperLimitPar;
    VfModuleParameter* m_pLowerLimitPar;
    VfModuleParameter* m_pResultUnit;
    VfModuleParameter* m_pRatingAct;
    VfModuleParameter* m_pMeasCountPar;
    VfModuleParameter* m_pMeasWait;
    VfModuleParameter* m_pMeasNumAct;
    VfModuleParameter* m_pMulCountAct;
    VfModuleParameter* m_pMulResultArray;
    VfModuleParameter* m_pMeasStartTime;
    VfModuleParameter* m_pMeasEndTime;

    VfModuleParameter* m_pClientNotifierPar;
    ClientActiveComponent m_ClientActiveNotifier;

    cStringValidator *m_pDutConstanstUnitValidator;
    QString m_sDutConstantUnit;
    // memorising DUT Constant scaling factor in case DUT constant changes
    double m_dutConstantScalingMem = 1;

    // vars dealing with error measurement
    TimerTemplateQtPtr m_ActualizeTimer; // after timed out we actualize progressvalue
    bool m_bFirstMeas = true;
    bool m_bMeasurementRunning = false;
    double m_fResult = 0.0; // error value in %
    ECALCRESULT::enResultTypes m_eRating = ECALCRESULT::RESULT_PASSED;
    quint32 m_nDUTPulseCounterStart = 0;
    quint32 m_nEnergyCounterFinal = 0;
    quint32 m_nEnergyCounterActual = 0;
    double m_fProgress = 0.0; // progress value in %
    double m_fEnergy = 0.0;
    quint32 m_nIntReg = 0;

    // Some decisions - we have enough of configration params around
    static constexpr quint32 m_nMulMeasStoredMax = 400;
    static constexpr quint32 m_nActualizeIntervallLowFreq = 1000;
    static constexpr quint32 m_nActualizeIntervallHighFreq = 50;

    // Multiple measurements
    qint32 m_nMeasurementsToGo;
    quint32 m_nMeasurementNo;
    QTimer m_WaitMultiTimer;
    QDateTime m_WaitStartDateTime;

    quint32 m_lastProgress = 0;
    QDateTime m_MeasStartDateTime;
    QDateTime m_MeasEndDateTime;

    MultipleResultHelper m_multipleResultHelper;
    cDoubleValidator *m_dutConstValidator;

    QElapsedTimer m_demoTimeSinceStart;
};
}

#endif // SEC1MODULEMEASPROGRAM_H
