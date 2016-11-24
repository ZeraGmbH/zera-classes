#ifndef BURDEN1MODULEMEASPROGRAM_H
#define BURDEN1MODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "burden1moduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"


class cBaseModule;
class cVeinModuleActvalue;
class cVeinModuleMetaData;
class cVeinModuleComponent;
class cVeinModuleParameter;

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


class cBurden1ModuleConfigData;
class cBurden1MeasDelegate;
class cBurden1Module;


class cBurden1ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cBurden1ModuleMeasProgram(cBurden1Module* module, cBurden1ModuleConfigData& configdata);
    virtual ~cBurden1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cBurden1Module* m_pModule;
    cBurden1ModuleConfigData& m_ConfigData;

    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pBRSCountInfo; // the number of Burden system we are configured for
    cVeinModuleComponent* m_pMeasureSignal;

    cVeinModuleParameter* m_pNominalRangeParameter;
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
    void setInterfaceActualValues(QVector<float> *actualValues);

    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void setMeasureSignal(int signal);

    void newNominalRange(QVariant nr);
    void newNominalBurden(QVariant nb);
    void newWireLength(QVariant wl);
    void newWireCrosssection(QVariant wc);
    void setParameters();
};

}
#endif // Burden1MODULEMEASPROGRAM_H
