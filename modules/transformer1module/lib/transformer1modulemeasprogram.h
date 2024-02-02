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
#include "foutinfo.h"


class cBaseModule;
class VfModuleActvalue;
class VfModuleMetaData;
class VfModuleComponent;
class VfModuleParameter;

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
    cTransformer1ModuleMeasProgram(cTransformer1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cTransformer1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cTransformer1ModuleConfigData* getConfData();

    cTransformer1Module* m_pModule;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pTRSCountInfo; // the number of transformer system we are configured for
    VfModuleComponent* m_pMeasureSignal;

    VfModuleParameter* m_pPrimClampPrimParameter;
    VfModuleParameter* m_pPrimClampSecParameter;
    VfModuleParameter* m_pSecClampPrimParameter;
    VfModuleParameter* m_pSecClampSecParameter;
    VfModuleParameter* m_pPrimDutParameter;
    VfModuleParameter* m_pSecDutParameter;

    QList<cTransformer1MeasDelegate*> m_Transformer1MeasDelegateList;

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

    void newPrimClampPrim(QVariant pcp);
    void newPrimClampSec(QVariant pcs);
    void newSecClampPrim(QVariant scp);
    void newSecClampSec(QVariant scs);
    void newPrimDut(QVariant pd);
    void newSecDut(QVariant sd);

    void setParameters();
};

}
#endif // TRANSFORMER1MODULEMEASPROGRAM_H
