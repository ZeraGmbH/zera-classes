#include "perveincomponentmeasuretransaction.h"
#include "scpicmdinfo.h"
#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <QVariant>
#include <QSequentialIterable>

namespace SCPIMODULE
{

int PerVeinComponentMeasureTransaction::m_instanceCount = 0;

PerVeinComponentMeasureTransaction::PerVeinComponentMeasureTransaction(const std::shared_ptr<QMultiHash<QString, PerVeinComponentMeasureTransaction *>> &moduleCommonPendingMeasureStore,
                                                                           const cSCPICmdInfoPtr &scpicmdinfo) :
    m_moduleCommonPendingMeasureStore(moduleCommonPendingMeasureStore),
    m_pSCPICmdInfo(scpicmdinfo)
{
    initialize();
    m_instanceCount++;
}

PerVeinComponentMeasureTransaction::PerVeinComponentMeasureTransaction(const PerVeinComponentMeasureTransaction &obj) :
    m_moduleCommonPendingMeasureStore(obj.m_moduleCommonPendingMeasureStore),
    m_pSCPICmdInfo(std::make_shared<cSCPICmdInfo>(*obj.m_pSCPICmdInfo))
{
    initialize();
    m_instanceCount++;
}

void PerVeinComponentMeasureTransaction::initialize()
{
    m_bInitPending = false;

    m_measureState.addTransition(this, &PerVeinComponentMeasureTransaction::measContinue, &m_measureConfigureState);
    m_measureConfigureState.addTransition(this, &PerVeinComponentMeasureTransaction::measContinue, &m_measureInitState);
    m_measureInitState.addTransition(this, &PerVeinComponentMeasureTransaction::measContinue, &m_measureFetchState);
    m_measureFetchState.addTransition(this, &PerVeinComponentMeasureTransaction::measContinue, &m_measureDoneState);
    m_MeasureStateMachine.addState(&m_measureState);
    m_MeasureStateMachine.addState(&m_measureConfigureState);
    m_MeasureStateMachine.addState(&m_measureInitState);
    m_MeasureStateMachine.addState(&m_measureFetchState);
    m_MeasureStateMachine.addState(&m_measureDoneState);
    connect(&m_measureState, &QState::entered, this, &PerVeinComponentMeasureTransaction::measure);
    connect(&m_measureConfigureState, &QState::entered, this, &PerVeinComponentMeasureTransaction::measureConfigure);
    connect(&m_measureInitState, &QState::entered, this, &PerVeinComponentMeasureTransaction::measureInit);
    connect(&m_measureFetchState, &QState::entered, this, &PerVeinComponentMeasureTransaction::measureFetch);
    m_MeasureStateMachine.setInitialState(&m_measureState);

    m_confConfigureState.addTransition(this, &PerVeinComponentMeasureTransaction::confContinue, &m_confConfigureDoneState);
    m_ConfigureStateMachine.addState(&m_confConfigureState);
    m_ConfigureStateMachine.addState(&m_confConfigureDoneState);
    connect(&m_confConfigureState, &QState::entered, this, &PerVeinComponentMeasureTransaction::configure);
    connect(&m_confConfigureDoneState, &QState::entered, this, &PerVeinComponentMeasureTransaction::configureDone);
    m_ConfigureStateMachine.setInitialState(&m_confConfigureState);

    m_readState.addTransition(this, &PerVeinComponentMeasureTransaction::readContinue, &m_readInitState);
    m_readInitState.addTransition(this, &PerVeinComponentMeasureTransaction::readContinue, &m_readFetchState);
    m_readFetchState.addTransition(this, &PerVeinComponentMeasureTransaction::readContinue, &m_readDoneState);
    m_ReadStateMachine.addState(&m_readState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    m_ReadStateMachine.addState(&m_readDoneState);
    connect(&m_readState, &QState::entered, this, &PerVeinComponentMeasureTransaction::read);
    connect(&m_readInitState, &QState::entered, this, &PerVeinComponentMeasureTransaction::readInit);
    connect(&m_readFetchState, &QState::entered, this, &PerVeinComponentMeasureTransaction::readFetch);
    m_ReadStateMachine.setInitialState(&m_readState);

    m_initInitState.addTransition(this, &PerVeinComponentMeasureTransaction::initContinue, &m_initDoneState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initDoneState);
    connect(&m_initInitState, &QState::entered, this, &PerVeinComponentMeasureTransaction::init);
    connect(&m_initDoneState, &QState::entered, this, &PerVeinComponentMeasureTransaction::initDone);
    m_InitStateMachine.setInitialState(&m_initInitState);

    m_fetchState.addTransition(this, &PerVeinComponentMeasureTransaction::fetchContinue, &m_fetchSyncState);
    m_fetchSyncState.addTransition(this, &PerVeinComponentMeasureTransaction::fetchContinue, &m_fetchFetchState);
    m_fetchFetchState.addTransition(this, &PerVeinComponentMeasureTransaction::fetchContinue, &m_fetchDoneState);
    m_FetchStateMachine.addState(&m_fetchState);
    m_FetchStateMachine.addState(&m_fetchSyncState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchDoneState);
    connect(&m_fetchState, &QState::entered, this, &PerVeinComponentMeasureTransaction::fetch);
    connect(&m_fetchSyncState, &QState::entered, this, &PerVeinComponentMeasureTransaction::fetchSync);
    connect(&m_fetchFetchState, &QState::entered, this, &PerVeinComponentMeasureTransaction::fetchFetch);
    m_FetchStateMachine.setInitialState(&m_fetchState);
}

PerVeinComponentMeasureTransaction::~PerVeinComponentMeasureTransaction()
{
    m_instanceCount--;
    m_moduleCommonPendingMeasureStore->remove(m_pSCPICmdInfo->componentOrRpcName, this);
}

enum signalCode {measCont, readCont, initCont, fetchCont};

void PerVeinComponentMeasureTransaction::receiveMeasureValue(const QVariant &value)
{
    m_moduleCommonPendingMeasureStore->remove(m_pSCPICmdInfo->componentOrRpcName, this);
    m_sAnswer = setAnswer(value);

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


void PerVeinComponentMeasureTransaction::execute(ScpiModelTypes modelType, const ScpiTransactionId &scpiTransactionId)
{
    switch (modelType)
    {
    case ScpiModelTypes::measure:
        if (!m_MeasureStateMachine.isRunning()) {
            m_measureScpiTransactionId = scpiTransactionId;
            m_MeasureStateMachine.start();
        }
        else
            qWarning("Measure state machine already running!");
        break;

    case ScpiModelTypes::configure:
        if (!m_ConfigureStateMachine.isRunning()) {
            m_configureScpiTransactionId = scpiTransactionId;
            m_ConfigureStateMachine.start();
        }
        else
            qWarning("Configure state machine already running!");
        break;

    case ScpiModelTypes::read:
        if (!m_ReadStateMachine.isRunning()) {
            m_readScpiTransactionId = scpiTransactionId;
            m_ReadStateMachine.start();
        }
        else
            qWarning("Model state machine already running!");
        break;

    case ScpiModelTypes::init:
        if (!m_InitStateMachine.isRunning()) {
            m_initScpiTransactionId = scpiTransactionId;
            m_InitStateMachine.start();
        }
        else
            qWarning("Init state machine already running!");
        break;

    case ScpiModelTypes::fetch:
        if (!m_FetchStateMachine.isRunning()) {
            m_fetchScpiTransactionId = scpiTransactionId;
            m_FetchStateMachine.start();
        }
        else
            qWarning("Fetch state machine already running!");
        break;
    }
}

int PerVeinComponentMeasureTransaction::entityID()
{
    return m_pSCPICmdInfo->entityId;
}

int PerVeinComponentMeasureTransaction::getInstanceCount()
{
    return m_instanceCount;
}

QString PerVeinComponentMeasureTransaction::convertVariantToString(const QVariant &value)
{
    if (value.canConvert<int>() || value.canConvert<double>())
        return QString("%1").arg(value.toDouble(), 0, 'g', 8);
    return value.toString();
}

QString PerVeinComponentMeasureTransaction::setAnswer(const QVariant &qvar)
{
    QString s;
    const QString unit = m_pSCPICmdInfo->veinComponentInfo["Unit"].toString();
    bool isString = QMetaType::type(qvar.typeName()) == QMetaType::type("QString");
    if (!isString && qvar.canConvert<QVariantList>()) {
        QSequentialIterable iterable = qvar.value<QSequentialIterable>();
        s = QString("%1:%2:[%3]:")
                .arg(m_pSCPICmdInfo->scpiModuleName, m_pSCPICmdInfo->scpiCommand, unit);
        for (const QVariant &v : iterable)
            s += (convertVariantToString(v) + ",");
        s = s.remove(s.size()-1, 1);
    }
    else
        s = QString("%1:%2:[%3]:%4").arg(m_pSCPICmdInfo->scpiModuleName,
                                         m_pSCPICmdInfo->scpiCommand,
                                         unit,
                                         convertVariantToString(qvar));
    return s;
}

void PerVeinComponentMeasureTransaction::measure()
{
    // this is our starting point ...nothing to do but better readable code
    emit measContinue();
}

void PerVeinComponentMeasureTransaction::measureConfigure()
{
    // for scpi compliance we have a configue but for the moment
    // we have noting to do here ... perhaps later
    emit measContinue();
}

void PerVeinComponentMeasureTransaction::measureInit()
{
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        m_moduleCommonPendingMeasureStore->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(measCont); // measure statemachine waits for measure value
}

void PerVeinComponentMeasureTransaction::measureFetch()
{
    emit sigMeasDone(m_sAnswer, m_measureScpiTransactionId, this);
    m_measureScpiTransactionId = ScpiTransactionId();
    emit measContinue();
}

void PerVeinComponentMeasureTransaction::configure()
{
    // for scpi compliance we have a configue but for the moment
    // we have noting to do here ... perhaps later
    emit confContinue();
}

void PerVeinComponentMeasureTransaction::configureDone()
{
    emit sigConfDone(m_configureScpiTransactionId, this);
}

void PerVeinComponentMeasureTransaction::read()
{
    // this is our starting point ...nothing to do but better readable code
    emit readContinue();
}

void PerVeinComponentMeasureTransaction::readInit()
{
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        m_moduleCommonPendingMeasureStore->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(readCont); // read statemachine waits for measure value
}

void PerVeinComponentMeasureTransaction::readFetch()
{
    emit sigReadDone(m_sAnswer, m_readScpiTransactionId, this);
    m_readScpiTransactionId = ScpiTransactionId();
    emit readContinue();
}

void PerVeinComponentMeasureTransaction::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value

    m_bInitPending = true;
    m_moduleCommonPendingMeasureStore->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(initCont); // init statemachine waits for measure value
}

void PerVeinComponentMeasureTransaction::initDone()
{
    m_bInitPending = false;
    emit sigInitDone(m_initScpiTransactionId, this);
}

void PerVeinComponentMeasureTransaction::fetch()
{
    // this is our starting point ...nothing to do but better readable code
    emit fetchContinue();
}

void PerVeinComponentMeasureTransaction::fetchSync()
{
    if (!m_bInitPending)
        emit fetchContinue();
    else
        signalList.append(fetchCont); // fetch statemachine waits for measure value
}

void PerVeinComponentMeasureTransaction::fetchFetch()
{
    emit sigFetchDone(m_sAnswer, m_fetchScpiTransactionId, this);
    m_fetchScpiTransactionId = ScpiTransactionId();
    emit fetchContinue();
}

}
