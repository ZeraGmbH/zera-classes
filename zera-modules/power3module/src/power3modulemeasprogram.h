#ifndef POWER3MODULEMEASPROGRAM_H
#define POWER3MODULEMEASPROGRAM_H

#include "power3moduleconfigdata.h"
#include "power3measdelegate.h"
#include <basemeasworkprogram.h>
#include <veinmoduleactvalue.h>
#include <veinmodulemetadata.h>
#include <QFinalState>
#include <memory>

namespace POWER3MODULE
{

enum power3moduleCmds
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

class cPower3Module;

class cPower3ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cPower3ModuleMeasProgram(cPower3Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cPower3ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cPower3ModuleConfigData* getConfData();

    cPower3Module* m_pModule;
    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pHPWCountInfo; // the number of values we produce
    cVeinModuleComponent* m_pMeasureSignal;

    QList<cPower3MeasDelegate*> m_Power3MeasDelegateList;

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
};

}
#endif // POWER3MODULEMEASPROGRAM_H
