#ifndef SFCMODULEMEASPROGRAM_H
#define SFCMODULEMEASPROGRAM_H

#include "secmeasinputdictionary.h"
#include "sfcmoduleconfigdata.h"
#include "vfmoduleparameter.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <secinterface.h>
#include <basemeasprogram.h>
#include <timertemplateqt.h>
#include "secresourcetypelist.h"

namespace SFCMODULE
{

namespace ECALCCMDID {
enum { COUNTEDGE = 1, COUNTRESET, ARM = 128};
}

enum sfcmoduleCmds
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

    stopmeas,

    readintregister,
    resetintregister,
    readvicount,

    setmastermux,
    setmastermeasmode,
    enableinterrupt,
    startmeasurement,

    actualizeprogress,
    test
};


class cSfcModule;
class cSfcModuleMeasProgram : public cBaseMeasProgram
{
    Q_OBJECT
public:
    cSfcModuleMeasProgram(cSfcModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    virtual ~cSfcModuleMeasProgram();
    void generateVeinInterface() override; // here we export our interface (entities)

signals:
    void interruptContinue();
    void setupContinue();
public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override;  // in interface are not updated when stop

private:
    cSfcModuleConfigData *getConfData();

    cSfcModule *m_pModule;
    std::unique_ptr<Zera::cSECInterface> m_pSECInterface;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClient* m_pSECClient;
    Zera::ProxyClient* m_pPCBClient;

    QList<VfModuleComponent *> m_veinActValueList; // the list of actual values we work on
    qint32 m_nIt;
    QString m_sIt;
    QString m_masterErrCalcName;
    QList<QString> m_sItList; // for interation over x Input hash
    SecResourceTypeList m_resourceTypeList;
    QHash<QString,QString> m_ResourceHash; // resourcetype, resourcelist ; seperated
    SecMeasInputDictionary m_dutInputDictionary;
    quint32 m_nIntReg = 0;
    bool m_bMeasurementRunning = false;
    quint32 m_nDUTPulseCounterStart = 0;
    quint32 m_measuredFlanks = 0;

    TimerTemplateQtPtr m_ActualizeTimer; // after timed out we actualize progressvalue
    static constexpr quint32 m_nActualizeIntervallLowFreq = 1000;

    // statemachine for deactivating
    QState m_stopECalculatorState; // we stop running measurement
    QState m_freeECalculatorState; // we give back our ecalcunits to sec server
    QState m_freeECResource; // and also give them back to the resource manager
    QFinalState m_deactivationDoneState;

    VfModuleParameter* m_pStartStopPar;
    VfModuleParameter* m_pFlankCountAct;
    VfModuleParameter* m_pLedStateAct;
    VfModuleParameter* m_pLedInitialStateAct;

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

    QState m_readDUTInputsState; // init to read all ref Input informations
    QState m_readDUTInputAliasState; // read for 1 Input
    QState m_readDUTInputDoneState;

    QState m_setpcbREFConstantNotifierState; // we get notified on refconstant changes
    QState m_setsecINTNotifierState; // we get notified on sec interrupts

    QFinalState m_activationDoneState; // and then we have finished

    // statemachine for starting error measurement
    QStateMachine m_startMeasurementMachine;
    QState m_setMasterMuxState; // we set the Input selectors
    QState m_setMasterMeasModeState; // and the meas modes
    QState m_enableInterruptState;
    QState m_startMeasurementState;
    QFinalState m_startMeasurementDoneState; // here we start it


    // statemachine for interrupthandling;
    QStateMachine m_InterrupthandlingStateMachine;
    QState m_readIntRegisterState;
    QFinalState m_FinalState;


    void handleSECInterrupt();
    void updateProgress(quint32 flankCountActual);
    void stopMeasurement(bool bAbort);

private slots:
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
    void readDUTInputs();
    void readDUTInputAlias();
    void readDUTInputDone();
    void setpcbREFConstantNotifier();
    void setsecINTNotifier();
    void activationDone();

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void deactivateMeasDone();
    void onStartStopChanged(QVariant newValue);

    void readIntRegister();
    void resetIntRegister();
    void checkForRestart();

    void stopECCalculator();
    void freeECalculator();
    void freeECResource();
    void deactivationDone();

    void setMasterMux();
    void setMasterMeasMode();
    void enableInterrupt();
    void startMeasurement();
    void startMeasurementDone();

    void Actualize();
};

}
#endif // SFCMODULEMEASPROGRAM_H
