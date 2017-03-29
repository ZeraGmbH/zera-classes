#include <QCoreApplication>
#include <QVariant>

#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>

#include "scpimodule.h"
#include "scpicmdinfo.h"
#include "scpimeasure.h"
#include "moduleinterface.h"


namespace SCPIMODULE
{

cSCPIMeasure::cSCPIMeasure(cSCPIModule *module, cSCPICmdInfo *scpicmdinfo)
    :m_pModule(module), m_pSCPICmdInfo(scpicmdinfo)
{
    initialize();
}


cSCPIMeasure::cSCPIMeasure(const cSCPIMeasure &obj)
{
    m_pModule = obj.m_pModule;
    m_pSCPICmdInfo = new cSCPICmdInfo(*obj.m_pSCPICmdInfo);
    initialize();
}


cSCPIMeasure::~cSCPIMeasure()
{
    m_pModule->scpiMeasureHash.remove(m_pSCPICmdInfo->componentName, this);
    delete m_pSCPICmdInfo;
}


void cSCPIMeasure::receiveMeasureValue(QVariant qvar)
{
    m_pModule->scpiMeasureHash.remove(m_pSCPICmdInfo->componentName, this);
    m_sAnswer = setAnswer(qvar);

    // we emit all expected signals
    for (int i = 0; i < signalList.count(); i++)
    {
        int sig;
        sig = signalList.at(i);
        switch (sig)
        {
        case measCont:
            emit measContinue();
            break;
        case readCont:
            emit readContinue();
            break;
        case initCont:
            emit initContinue();
            break;
        case fetchCont:
            emit fetchContinue();
            break;
        }
    }

    signalList.clear();
}


void cSCPIMeasure::execute(quint8 cmd)
{
    switch (cmd)
    {
    case SCPIModelType::measure:
        m_MeasureStateMachine.start();
        break;

    case SCPIModelType::configure:
        m_ConfigureStateMachine.start();
        break;

    case SCPIModelType::read:
        m_ReadStateMachine.start();
        break;

    case SCPIModelType::init:
        m_InitStateMachine.start();
        break;

    case SCPIModelType::fetch:
        m_FetchStateMachine.start();
        break;
    }
}


int cSCPIMeasure::entityID()
{
    return m_pSCPICmdInfo->entityId;
}


QString cSCPIMeasure::setAnswer(QVariant qvar)
{
    QString s;

    if (qvar.canConvert<QVariantList>())
    {
        QSequentialIterable iterable = qvar.value<QSequentialIterable>();

        s = QString("%1:%2:[%3]:").arg(m_pSCPICmdInfo->scpiModuleName, m_pSCPICmdInfo->scpiCommand, m_pSCPICmdInfo->unit);
        foreach (const QVariant &v, iterable)
            s += (v.toString()+",");
        s = s.remove(s.count()-1, 1);

    }
    else
        s = QString("%1:%2:[%3]:%4").arg(m_pSCPICmdInfo->scpiModuleName, m_pSCPICmdInfo->scpiCommand, m_pSCPICmdInfo->unit).arg(qvar.toString());

    return (s);
}


void cSCPIMeasure::measure()
{
    // this is our starting point ...nothing to do but better readable code
    emit measContinue();
}


void cSCPIMeasure::measureConfigure()
{
    // for scpi compliance we have a configue but for the moment
    // we have noting to do here ... perhaps later
    emit measContinue();
}


void cSCPIMeasure::measureInit()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    m_pModule->scpiMeasureHash.insert(m_pSCPICmdInfo->componentName, this);

    signalList.append(measCont); // measure statemachine waits for measure value
}


void cSCPIMeasure::measureFetch()
{
    emit sigMeasDone(m_sAnswer);
    emit measContinue();
}


void cSCPIMeasure::measureDone()
{
    // only finished statemachine
}


void cSCPIMeasure::configure()
{
    // for scpi compliance we have a configue but for the moment
    // we have noting to do here ... perhaps later
    emit confContinue();
}


void cSCPIMeasure::configureDone()
{
    emit sigConfDone();
}


void cSCPIMeasure::read()
{
    // this is our starting point ...nothing to do but better readable code
    emit readContinue();
}


void cSCPIMeasure::readInit()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    m_pModule->scpiMeasureHash.insert(m_pSCPICmdInfo->componentName, this);

    signalList.append(readCont); // read statemachine waits for measure value
}

void cSCPIMeasure::readFetch()
{
    emit sigReadDone(m_sAnswer);
    emit readContinue();
}


void cSCPIMeasure::readDone()
{
    // only finished statemachine
}


void cSCPIMeasure::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value
    m_pModule->scpiMeasureHash.insert(m_pSCPICmdInfo->componentName, this);
    m_bInitPending = true; //

    signalList.append(initCont); // init statemachine waits for measure value
}


void cSCPIMeasure::initDone()
{
    m_bInitPending = false;
    emit sigInitDone();
}


void cSCPIMeasure::fetch()
{
    // this is our starting point ...nothing to do but better readable code
    emit fetchContinue();
}


void cSCPIMeasure::fetchSync()
{
    if (!m_bInitPending)
        emit fetchContinue();
    else
        signalList.append(fetchCont); // fetch statemachine waits for measure value
}


void cSCPIMeasure::fetchFetch()
{
    emit sigFetchDone(m_sAnswer);
    emit fetchContinue();
}


void cSCPIMeasure::fetchDone()
{
    // only finished statemachine
}


void cSCPIMeasure::initialize()
{
    m_bInitPending = false;

    m_measureState.addTransition(this, SIGNAL(measContinue()), &m_measureConfigureState);
    m_measureConfigureState.addTransition(this, SIGNAL(measContinue()), &m_measureInitState);
    m_measureInitState.addTransition(this, SIGNAL(measContinue()), &m_measureFetchState);
    m_measureFetchState.addTransition(this, SIGNAL(measContinue()), &m_measureDoneState);
    m_MeasureStateMachine.addState(&m_measureState);
    m_MeasureStateMachine.addState(&m_measureConfigureState);
    m_MeasureStateMachine.addState(&m_measureInitState);
    m_MeasureStateMachine.addState(&m_measureFetchState);
    m_MeasureStateMachine.addState(&m_measureDoneState);
    connect(&m_measureState, SIGNAL(entered()), SLOT(measure()));
    connect(&m_measureConfigureState, SIGNAL(entered()), SLOT(measureConfigure()));
    connect(&m_measureInitState, SIGNAL(entered()), SLOT(measureInit()));
    connect(&m_measureFetchState, SIGNAL(entered()), SLOT(measureFetch()));
    connect(&m_measureDoneState, SIGNAL(entered()), SLOT(measureDone()));
    m_MeasureStateMachine.setInitialState(&m_measureState);

    m_confConfigureState.addTransition(this, SIGNAL(confContinue()), &m_confConfigureDoneState);
    m_ConfigureStateMachine.addState(&m_confConfigureState);
    m_ConfigureStateMachine.addState(&m_confConfigureDoneState);
    connect(&m_confConfigureState, SIGNAL(entered()), SLOT(configure()));
    connect(&m_confConfigureDoneState, SIGNAL(entered()), SLOT(configureDone()));
    m_ConfigureStateMachine.setInitialState(&m_confConfigureState);

    m_readState.addTransition(this, SIGNAL(readContinue()), &m_readInitState);
    m_readInitState.addTransition(this, SIGNAL(readContinue()), &m_readFetchState);
    m_readFetchState.addTransition(this, SIGNAL(readContinue()), &m_readDoneState);
    m_ReadStateMachine.addState(&m_readState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    m_ReadStateMachine.addState(&m_readDoneState);
    connect(&m_readState, SIGNAL(entered()), SLOT(read()));
    connect(&m_readInitState, SIGNAL(entered()), SLOT(readInit()));
    connect(&m_readFetchState, SIGNAL(entered()), SLOT(readFetch()));
    connect(&m_readDoneState, SIGNAL(entered()), SLOT(readDone()));
    m_ReadStateMachine.setInitialState(&m_readState);

    m_initInitState.addTransition(this, SIGNAL(initContinue()), &m_initDoneState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initDoneState);
    connect(&m_initInitState, SIGNAL(entered()), SLOT(init()));
    connect(&m_initDoneState, SIGNAL(entered()), SLOT(initDone()));
    m_InitStateMachine.setInitialState(&m_initInitState);

    m_fetchState.addTransition(this, SIGNAL(fetchContinue()), &m_fetchSyncState);
    m_fetchSyncState.addTransition(this, SIGNAL(fetchContinue()), &m_fetchFetchState);
    m_fetchFetchState.addTransition(this, SIGNAL(fetchContinue()), &m_fetchDoneState);
    m_FetchStateMachine.addState(&m_fetchState);
    m_FetchStateMachine.addState(&m_fetchSyncState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchDoneState);
    connect(&m_fetchState, SIGNAL(entered()), SLOT(fetch()));
    connect(&m_fetchSyncState, SIGNAL(entered()), SLOT(fetchSync()));
    connect(&m_fetchFetchState, SIGNAL(entered()), SLOT(fetchFetch()));
    connect(&m_fetchDoneState, SIGNAL(entered()), SLOT(fetchDone()));
    m_FetchStateMachine.setInitialState(&m_fetchState);
}


}
