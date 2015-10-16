#ifndef POWER3MODULEMEASPROGRAM_H
#define POWER3MODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "power3moduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"


class cModuleSignal;
class cModuleInfo;
class cBaseModule;

class VeinPeer;
class VeinEntity;
class cPower3MeasDelegate;
class QStateMachine;
class QState;
class QFinalState;


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


class cPower3ModuleConfigData;
class cPower3Module;


class cPower3ModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cPower3ModuleMeasProgram(cPower3Module* module, VeinPeer* peer, cPower3ModuleConfigData& configdata);
    virtual ~cPower3ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &jsArr);

public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cPower3Module* m_pModule;
    cPower3ModuleConfigData& m_ConfigData;

    cModuleInfo* m_pHPWCountInfo; // the number of values we produce
    QList<VeinEntity*> m_EntityNameHPWList; // we have a list for all pqs names for a power meter
    QList<VeinEntity*> m_EntityActValueHPWList; // we have a list for all pgs values for a power meter
    QList<cPower3MeasDelegate*> m_Power3MeasDelegateList;

    cModuleSignal* m_pMeasureSignal;

    // statemachine for activating gets the following states
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

    void setActualValuesNames();

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void searchActualValues();
    void activateDone();

    void deactivateMeas();
    void deactivateMeasDone();
};

}
#endif // POWER3MODULEMEASPROGRAM_H
