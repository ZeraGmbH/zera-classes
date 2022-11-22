#ifndef SEC1MODULEMEASPROGRAM_H
#define SEC1MODULEMEASPROGRAM_H

#include "sec1module.h"
#include "sec1moduleconfigdata.h"
#include "secinputinfo.h"
#include "ecalcinfo.h"
#include "clientactivecomponent.h"
#include "multipleresulthelper.h"
#include <stringvalidator.h>
#include <basemeasprogram.h>
#include <QDateTime>

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
    readrefInputalias,
    readdutInputalias,
    setpcbrefconstantnotifier,
    setsecintnotifier,

    freeecalcunits,
    freeecresource,
    freeusermem,

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
    fetchrefconstant,

    readintregister,
    resetintregister,
    readvicount

};

#define irqPCBNotifier 16

class cSec1ModuleMeasProgram: public cBaseMeasProgram
{
    Q_OBJECT
public:
    cSec1ModuleMeasProgram(cSec1Module* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cSec1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cSec1ModuleConfigData* getConfData();

    cSec1Module* m_pModule; // the module we live in
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

    Zera::Proxy::ProxyClientPtr m_rmClient;
    Zera::Proxy::cProxyClient* m_pSECClient;
    Zera::Proxy::cProxyClient* m_pPCBClient;

    QStringList m_ResourceTypeList;
    QHash<QString,QString> m_ResourceHash; // resourcetype, resourcelist ; seperated
    QHash<QString,cSecInputInfo*> mREFSecInputInfoHash; // we hold a list of all our Input properties
    QHash<QString,cSecInputInfo*> mDUTSecInputInfoHash; // systemname from configfile->alias, csecInputinfo
    QHash<QString,cSecInputInfo*> mREFSecInputSelectionHash;
    QHash<QString,cSecInputInfo*> mDUTSecInputSelectionHash;

    QStringList m_REFAliasList; // we want to have an ordered list with Input alias
    QStringList m_DUTAliasList;
    qint32 m_nIt;
    QList<QString> m_sItList; // for interation over x Input hash
    cSecInputInfo* siInfo;
    QString m_sIt;

    cECalcChannelInfo m_MasterEcalculator;
    cECalcChannelInfo m_SlaveEcalculator;

    cVeinModuleParameter* m_pModePar;
    cVeinModuleParameter* m_pDutInputPar;
    cVeinModuleParameter* m_pRefInputPar;
    cVeinModuleParameter* m_pRefConstantPar;
    cVeinModuleParameter* m_pDutConstantPar;
    cVeinModuleParameter* m_pDutConstantUScaleDenom;
    cVeinModuleParameter* m_pDutConstantUScaleNum;
    cVeinModuleParameter* m_pDutConstantIScaleDenom;
    cVeinModuleParameter* m_pDutTypeMeasurePoint;
    cVeinModuleParameter* m_pDutConstantIScaleNum;
    cVeinModuleParameter* m_pDutConstantUnitPar;
    cVeinModuleParameter* m_pMRatePar;
    cVeinModuleParameter* m_pTargetPar;
    cVeinModuleParameter* m_pEnergyPar;
    cVeinModuleParameter* m_pStartStopPar;
    cVeinModuleParameter* m_pStatusAct;
    cVeinModuleParameter* m_pProgressAct;
    cVeinModuleParameter* m_pEnergyAct;
    cVeinModuleParameter* m_pEnergyFinalAct;
    cVeinModuleParameter* m_pResultAct;
    cVeinModuleParameter* m_pRefFreqInput;
    cVeinModuleParameter* m_pContinuousPar;
    cVeinModuleParameter* m_pUpperLimitPar;
    cVeinModuleParameter* m_pLowerLimitPar;
    cVeinModuleParameter* m_pResultUnit;
    cVeinModuleParameter* m_pRatingAct;
    cVeinModuleParameter* m_pMeasCountPar;
    cVeinModuleParameter* m_pMeasWait;
    cVeinModuleParameter* m_pMeasNumAct;
    cVeinModuleParameter* m_pMulCountAct;
    cVeinModuleParameter* m_pMulResultArray;

    cVeinModuleParameter* m_pClientNotifierPar;
    ClientActiveComponent m_ClientActiveNotifier;


    cStringValidator *m_pDutConstanstUnitValidator;
    QString m_sDutConstantUnit;
    // memorising DUT Constant scaling factor in case DUT constant changes
    double m_dutConstantScalingMem = 1;

    // vars dealing with error measurement
    bool m_bFirstMeas;
    bool m_bMeasurementRunning = false;
    QTimer m_ActualizeTimer; // after timed out we actualize progressvalue
    quint32 m_nStatus; // idle, started, running, finished
    double m_fResult; // error value in %
    ECALCRESULT::enResultTypes m_eRating;
    quint32 m_nDUTPulseCounterStart;
    quint32 m_nDUTPulseCounterActual;
    quint32 m_nEnergyCounterFinal;
    quint32 m_nEnergyCounterActual;
    double m_fProgress; // progress value in %
    double m_fEnergy;
    quint32 m_nIntReg;

    // Some decisions - we have enough of configration params around
    static constexpr quint32 m_nMulMeasStoredMax = 400;
    static constexpr quint32 m_nActualizeIntervallLowFreq = 1000;
    static constexpr quint32 m_nActualizeIntervallHighFreq = 50;

    // Multiple measurements
    qint32 m_nMeasurementsToGo;
    quint32 m_nMeasurementNo;
    QTimer m_WaitMultiTimer;
    QDateTime m_WaitStartDateTime;

    MultipleResultHelper m_multipleResultHelper;

    // methods
    void setInterfaceComponents();
    void setValidators();

    QStringList getDutConstUnitValidator();
    QString getEnergyUnit();
    void initDutConstantUnit(QStringList sl);
    void initDutConstantUnit();

    void handleChangedREFConst();
    void handleSECInterrupt();
    void cmpDependencies();
    void stopMeasurement(bool bAbort);

    const QString multiResultToJson();
    void multiResultToVein();
    double getUnitFactor();
    QString getRefInputDisplayString(QString inputName);

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
    void newMode(QVariant mode);
    void newDutConstant(QVariant dutconst);
    /**
     * @brief newDutConstantScale
     *
     * caluclates dut constatn scale factor from
     * PAR_DutConstantUScaleNum
     * PAR_DutConstantUScaleDenom
     * PAR_DutConstantIScaleNum
     * PAR_DutConstantIScaleDenom
     *
     * is also connected to m_pDutTypeMeasurePoint
     *
     * @param uINumDenomValue: new Num or denum value
     * @param numDenomId: (0): is Numerator (1 or ...) is Denominator
     * @param transType: Transformer type (U) Voltage (I) Current
     */
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
    bool found(QList<cSec1ModuleConfigData::TRefInput> &list, QString searched);
};
}

#endif // SEC1MODULEMEASPROGRAM_H
