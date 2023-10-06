#ifndef LAMBDAMODULEMEASPROGRAM_H
#define LAMBDAMODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "lambdamoduleconfigdata.h"
#include "basemeasworkprogram.h"

class cBaseModule;
class VfModuleActvalue;
class VfModuleMetaData;
class VfModuleComponent;


class QStateMachine;
class QState;
class QFinalState;


namespace LAMBDAMODULE
{

enum lambdamoduleCmds
{
    resourcemanagerconnect,
    sendrmident,
    readresourcetypes,
    readresourcesense,
    readresourcesenseinfos,
    readresourcesenseinfo,

    readresourcesource,
    readresourcessourceinfos,
    readresourcesourceinfo,
    claimresourcesource,

    pcbserverconnect,
    readsamplerate,

    readsensechannelalias,
    readsensechannelunit,
    readsensechanneldspchannel,

    readsourcechannelalias,
    readsourcechanneldspchannel,
    readsourceformfactor,

    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
    freepgrmem,
    freeusermem,
    freeresourcesource,
    unregisterrangenotifiers,

    readurvalue,
    setfrequencyscales,
    setchannelrangenotifier
};


class cLambdaModuleConfigData;
class cLambdaMeasDelegate;
class cLambdaModule;


class cLambdaModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cLambdaModuleMeasProgram(cLambdaModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cLambdaModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cLambdaModuleConfigData* getConfData();

    cLambdaModule* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pLAMBDACountInfo; // the number of values we produce
    VfModuleComponent* m_pMeasureSignal;

    QList<cLambdaMeasDelegate*> m_LambdaMeasDelegateList;

    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

private slots:
    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void setMeasureSignal(int signal);
    void computeInput2Sum(QVariant value);
};

}
#endif // LAMBDAMODULEMEASPROGRAM_H
