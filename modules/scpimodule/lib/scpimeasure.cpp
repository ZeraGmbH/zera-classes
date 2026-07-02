#include "scpimeasure.h"
#include "scpimeasuredelegate.h"
#include "scpicmdinfo.h"
#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <QVariant>
#include <QSequentialIterable>

namespace SCPIMODULE
{

int cSCPIMeasure::m_instanceCount = 0;

cSCPIMeasure::cSCPIMeasure(const std::shared_ptr<QMultiHash<QString, cSCPIMeasure *>> &scpiMeasureHash, const cSCPICmdInfoPtr &scpicmdinfo) :
    m_scpiMeasureHash(scpiMeasureHash),
    m_pSCPICmdInfo(scpicmdinfo)
{
    initialize();
    m_instanceCount++;
}

cSCPIMeasure::cSCPIMeasure(const cSCPIMeasure &obj) :
    m_scpiMeasureHash(obj.m_scpiMeasureHash),
    m_pSCPICmdInfo(std::make_shared<cSCPICmdInfo>(*obj.m_pSCPICmdInfo))
{
    initialize();
    m_instanceCount++;
}

cSCPIMeasure::~cSCPIMeasure()
{
    m_instanceCount--;
    m_scpiMeasureHash->remove(m_pSCPICmdInfo->componentOrRpcName, this);
}

void cSCPIMeasure::receiveMeasureValue(const QVariant &value)
{
    m_scpiMeasureHash->remove(m_pSCPICmdInfo->componentOrRpcName, this);
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


void cSCPIMeasure::execute(quint8 cmd, const ScpiTransactionId &scpiTransactionId)
{
    switch (cmd)
    {
    case ScpiModelType::measure:
        if (!m_MeasureStateMachine.isRunning()) {
            m_measureScpiTransactionId = scpiTransactionId;
            m_MeasureStateMachine.start();
        }
        break;

    case ScpiModelType::configure:
        if (!m_ConfigureStateMachine.isRunning()) {
            m_configureScpiTransactionId = scpiTransactionId;
            m_ConfigureStateMachine.start();
        }
        break;

    case ScpiModelType::read:
        if (!m_ReadStateMachine.isRunning()) {
            m_readScpiTransactionId = scpiTransactionId;
            m_ReadStateMachine.start();
        }
        break;

    case ScpiModelType::init:
        if (!m_InitStateMachine.isRunning()) {
            m_initScpiTransactionId = scpiTransactionId;
            m_InitStateMachine.start();
        }
        break;

    case ScpiModelType::fetch:
        if (!m_FetchStateMachine.isRunning()) {
            m_fetchScpiTransactionId = scpiTransactionId;
            m_FetchStateMachine.start();
        }
        break;
    }
}


int cSCPIMeasure::entityID()
{
    return m_pSCPICmdInfo->entityId;
}

int cSCPIMeasure::getInstanceCount()
{
    return m_instanceCount;
}

QString cSCPIMeasure::convertVariantToString(const QVariant &value)
{
    if (value.canConvert<int>() || value.canConvert<double>())
        return QString("%1").arg(value.toDouble(), 0, 'g', 8);
    return value.toString();
}

QString cSCPIMeasure::setAnswer(const QVariant &qvar)
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
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        m_scpiMeasureHash->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(measCont); // measure statemachine waits for measure value
}


void cSCPIMeasure::measureFetch()
{
    emit sigMeasDone(m_sAnswer, m_measureScpiTransactionId, this);
    m_measureScpiTransactionId = ScpiTransactionId();
    emit measContinue();
}


void cSCPIMeasure::configure()
{
    // for scpi compliance we have a configue but for the moment
    // we have noting to do here ... perhaps later
    emit confContinue();
}


void cSCPIMeasure::configureDone()
{
    emit sigConfDone(m_configureScpiTransactionId, this);
}


void cSCPIMeasure::read()
{
    // this is our starting point ...nothing to do but better readable code
    emit readContinue();
}


void cSCPIMeasure::readInit()
{
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        m_scpiMeasureHash->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(readCont); // read statemachine waits for measure value
}

void cSCPIMeasure::readFetch()
{
    emit sigReadDone(m_sAnswer, m_readScpiTransactionId, this);
    m_readScpiTransactionId = ScpiTransactionId();
    emit readContinue();
}


void cSCPIMeasure::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value

    m_bInitPending = true;
    m_scpiMeasureHash->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(initCont); // init statemachine waits for measure value
}


void cSCPIMeasure::initDone()
{
    m_bInitPending = false;
    emit sigInitDone(m_initScpiTransactionId, this);
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
    emit sigFetchDone(m_sAnswer, m_fetchScpiTransactionId, this);
    m_fetchScpiTransactionId = ScpiTransactionId();
    emit fetchContinue();
}


void cSCPIMeasure::initialize()
{
    m_bInitPending = false;

    m_measureState.addTransition(this, &cSCPIMeasure::measContinue, &m_measureConfigureState);
    m_measureConfigureState.addTransition(this, &cSCPIMeasure::measContinue, &m_measureInitState);
    m_measureInitState.addTransition(this, &cSCPIMeasure::measContinue, &m_measureFetchState);
    m_measureFetchState.addTransition(this, &cSCPIMeasure::measContinue, &m_measureDoneState);
    m_MeasureStateMachine.addState(&m_measureState);
    m_MeasureStateMachine.addState(&m_measureConfigureState);
    m_MeasureStateMachine.addState(&m_measureInitState);
    m_MeasureStateMachine.addState(&m_measureFetchState);
    m_MeasureStateMachine.addState(&m_measureDoneState);
    connect(&m_measureState, &QState::entered, this, &cSCPIMeasure::measure);
    connect(&m_measureConfigureState, &QState::entered, this, &cSCPIMeasure::measureConfigure);
    connect(&m_measureInitState, &QState::entered, this, &cSCPIMeasure::measureInit);
    connect(&m_measureFetchState, &QState::entered, this, &cSCPIMeasure::measureFetch);
    m_MeasureStateMachine.setInitialState(&m_measureState);

    m_confConfigureState.addTransition(this, &cSCPIMeasure::confContinue, &m_confConfigureDoneState);
    m_ConfigureStateMachine.addState(&m_confConfigureState);
    m_ConfigureStateMachine.addState(&m_confConfigureDoneState);
    connect(&m_confConfigureState, &QState::entered, this, &cSCPIMeasure::configure);
    connect(&m_confConfigureDoneState, &QState::entered, this, &cSCPIMeasure::configureDone);
    m_ConfigureStateMachine.setInitialState(&m_confConfigureState);

    m_readState.addTransition(this, &cSCPIMeasure::readContinue, &m_readInitState);
    m_readInitState.addTransition(this, &cSCPIMeasure::readContinue, &m_readFetchState);
    m_readFetchState.addTransition(this, &cSCPIMeasure::readContinue, &m_readDoneState);
    m_ReadStateMachine.addState(&m_readState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    m_ReadStateMachine.addState(&m_readDoneState);
    connect(&m_readState, &QState::entered, this, &cSCPIMeasure::read);
    connect(&m_readInitState, &QState::entered, this, &cSCPIMeasure::readInit);
    connect(&m_readFetchState, &QState::entered, this, &cSCPIMeasure::readFetch);
    m_ReadStateMachine.setInitialState(&m_readState);

    m_initInitState.addTransition(this, &cSCPIMeasure::initContinue, &m_initDoneState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initDoneState);
    connect(&m_initInitState, &QState::entered, this, &cSCPIMeasure::init);
    connect(&m_initDoneState, &QState::entered, this, &cSCPIMeasure::initDone);
    m_InitStateMachine.setInitialState(&m_initInitState);

    m_fetchState.addTransition(this, &cSCPIMeasure::fetchContinue, &m_fetchSyncState);
    m_fetchSyncState.addTransition(this, &cSCPIMeasure::fetchContinue, &m_fetchFetchState);
    m_fetchFetchState.addTransition(this, &cSCPIMeasure::fetchContinue, &m_fetchDoneState);
    m_FetchStateMachine.addState(&m_fetchState);
    m_FetchStateMachine.addState(&m_fetchSyncState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchDoneState);
    connect(&m_fetchState, &QState::entered, this, &cSCPIMeasure::fetch);
    connect(&m_fetchSyncState, &QState::entered, this, &cSCPIMeasure::fetchSync);
    connect(&m_fetchFetchState, &QState::entered, this, &cSCPIMeasure::fetchFetch);
    m_FetchStateMachine.setInitialState(&m_fetchState);
}

}
