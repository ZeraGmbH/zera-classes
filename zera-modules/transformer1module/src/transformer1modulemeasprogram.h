#ifndef TRANSFORMER1MODULEMEASPROGRAM_H
#define TRANSFORMER1MODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "transformer1moduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"


class cBaseModule;
class cVeinModuleActvalue;
class cVeinModuleMetaData;
class cVeinModuleComponent;
class cVeinModuleParameter;

class QStateMachine;
class QState;
class QFinalState;


namespace TRANSFORMER1MODULE
{

enum transformer1moduleCmds
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


class cTransformer1ModuleConfigData;
class cTransformer1MeasDelegate;
class cTransformer1Module;


class cTransformer1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cTransformer1ModuleMeasProgram(cTransformer1Module* module, cTransformer1ModuleConfigData& configdata);
    virtual ~cTransformer1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cTransformer1Module* m_pModule;
    cTransformer1ModuleConfigData& m_ConfigData;

    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pTRSCountInfo; // the number of transformer system we are configured for
    cVeinModuleComponent* m_pMeasureSignal;

    cVeinModuleParameter* m_pPrimClampPrimParameter;
    cVeinModuleParameter* m_pPrimClampSecParameter;
    cVeinModuleParameter* m_pSecClampPrimParameter;
    cVeinModuleParameter* m_pSecClampSecParameter;
    cVeinModuleParameter* m_pPrimDutParameter;
    cVeinModuleParameter* m_pSecDutParameter;

    QList<cTransformer1MeasDelegate*> m_Transformer1MeasDelegateList;

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
#endif // TRANSFORMER1MODULEMEASPROGRAM_H
