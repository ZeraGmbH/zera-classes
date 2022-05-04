#ifndef BURDEN1MODULEMEASPROGRAM_H
#define BURDEN1MODULEMEASPROGRAM_H

#include "burden1moduleconfigdata.h"
#include "burden1measdelegate.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"
#include <basemeasworkprogram.h>
#include <veinmoduleparameter.h>
#include <veinmodulemetadata.h>
#include <QFinalState>

namespace BURDEN1MODULE
{

enum Burden1moduleCmds
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

class cBurden1Module;

class cBurden1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cBurden1ModuleMeasProgram(cBurden1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cBurden1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cBurden1ModuleConfigData* getConfData();

    cBurden1Module* m_pModule;
    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pBRSCountInfo; // the number of Burden system we are configured for
    cVeinModuleComponent* m_pMeasureSignal;

    cVeinModuleParameter* m_pNominalRangeParameter;
    cVeinModuleParameter* m_pNominalRangeFactorParameter;
    cVeinModuleParameter* m_pNominalBurdenParameter;
    cVeinModuleParameter* m_pWireLengthParameter;
    cVeinModuleParameter* m_pWireCrosssectionParameter;

    QList<cBurden1MeasDelegate*> m_Burden1MeasDelegateList;

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

    void newNominalRange(QVariant nr);
    void newNominalFactorRange(QVariant nrf);
    void newNominalBurden(QVariant nb);
    void newWireLength(QVariant wl);
    void newWireCrosssection(QVariant wc);
    void setParameters();
};

}
#endif // Burden1MODULEMEASPROGRAM_H
