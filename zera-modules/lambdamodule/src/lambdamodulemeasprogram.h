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
class cVeinModuleActvalue;
class cVeinModuleMetaData;
class cVeinModuleComponent;


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
    cLambdaModuleMeasProgram(cLambdaModule* module, cLambdaModuleConfigData& configdata);
    virtual ~cLambdaModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cLambdaModule* m_pModule;
    cLambdaModuleConfigData& m_ConfigData;

    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pLAMBDACountInfo; // the number of values we produce
    cVeinModuleComponent* m_pMeasureSignal;

    QList<cLambdaMeasDelegate*> m_LambdaMeasDelegateList;

    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void setMeasureSignal(int signal);
};

}
#endif // LAMBDAMODULEMEASPROGRAM_H
