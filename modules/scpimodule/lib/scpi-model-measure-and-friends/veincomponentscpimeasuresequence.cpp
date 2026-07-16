#include "veincomponentscpimeasuresequence.h"
#include "scpicmdinfo.h"
#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <QVariant>
#include <QSequentialIterable>

namespace SCPIMODULE
{

int VeinComponentScpiMeasureSequence::m_instanceCount = 0;

QMultiHash<QString, VeinComponentScpiMeasureSequence *> *VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()
{
    static QMultiHash<QString, VeinComponentScpiMeasureSequence *> *veinComponentScpiMeasureSequenceStoreSingleton = new QMultiHash<QString, VeinComponentScpiMeasureSequence *>();
    return veinComponentScpiMeasureSequenceStoreSingleton;
}

VeinComponentScpiMeasureSequence::VeinComponentScpiMeasureSequence(const cSCPICmdInfoPtr &scpicmdinfo) :
    m_pSCPICmdInfo(scpicmdinfo)
{
    initialize();
    m_instanceCount++;
}

VeinComponentScpiMeasureSequence::VeinComponentScpiMeasureSequence(const VeinComponentScpiMeasureSequence &obj) :
    m_pSCPICmdInfo(std::make_shared<cSCPICmdInfo>(*obj.m_pSCPICmdInfo))
{
    initialize();
    m_instanceCount++;
}

void VeinComponentScpiMeasureSequence::initialize()
{
    m_bInitPending = false;

    m_measureInitState.addTransition(this, &VeinComponentScpiMeasureSequence::measContinue, &m_measureFetchState);
    m_measureFetchState.addTransition(this, &VeinComponentScpiMeasureSequence::measContinue, &m_measureDoneState);
    m_MeasureStateMachine.addState(&m_measureInitState);
    m_MeasureStateMachine.addState(&m_measureFetchState);
    m_MeasureStateMachine.addState(&m_measureDoneState);
    connect(&m_measureInitState, &QState::entered, this, &VeinComponentScpiMeasureSequence::measureInit);
    connect(&m_measureFetchState, &QState::entered, this, &VeinComponentScpiMeasureSequence::measureFetch);
    m_MeasureStateMachine.setInitialState(&m_measureInitState);

    m_ConfigureStateMachine.addState(&m_confConfigureDoneState);
    connect(&m_confConfigureDoneState, &QState::entered, this, &VeinComponentScpiMeasureSequence::configureDone);
    m_ConfigureStateMachine.setInitialState(&m_confConfigureDoneState);

    m_readInitState.addTransition(this, &VeinComponentScpiMeasureSequence::readContinue, &m_readFetchState);
    m_readFetchState.addTransition(this, &VeinComponentScpiMeasureSequence::readContinue, &m_readDoneState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    m_ReadStateMachine.addState(&m_readDoneState);
    connect(&m_readInitState, &QState::entered, this, &VeinComponentScpiMeasureSequence::readInit);
    connect(&m_readFetchState, &QState::entered, this, &VeinComponentScpiMeasureSequence::readFetch);
    m_ReadStateMachine.setInitialState(&m_readInitState);

    m_initInitState.addTransition(this, &VeinComponentScpiMeasureSequence::initContinue, &m_initDoneState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initDoneState);
    connect(&m_initInitState, &QState::entered, this, &VeinComponentScpiMeasureSequence::init);
    connect(&m_initDoneState, &QState::entered, this, &VeinComponentScpiMeasureSequence::initDone);
    m_InitStateMachine.setInitialState(&m_initInitState);

    m_fetchSyncState.addTransition(this, &VeinComponentScpiMeasureSequence::fetchContinue, &m_fetchFetchState);
    m_fetchFetchState.addTransition(this, &VeinComponentScpiMeasureSequence::fetchContinue, &m_fetchDoneState);
    m_FetchStateMachine.addState(&m_fetchSyncState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchDoneState);
    connect(&m_fetchSyncState, &QState::entered, this, &VeinComponentScpiMeasureSequence::fetchSync);
    connect(&m_fetchFetchState, &QState::entered, this, &VeinComponentScpiMeasureSequence::fetchFetch);
    m_FetchStateMachine.setInitialState(&m_fetchSyncState);
}

VeinComponentScpiMeasureSequence::~VeinComponentScpiMeasureSequence()
{
    m_instanceCount--;
    VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->remove(m_pSCPICmdInfo->componentOrRpcName, this);
}

enum signalCode {measCont, readCont, initCont, fetchCont};

void VeinComponentScpiMeasureSequence::receiveMeasureValue(const QVariant &value)
{
    VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->remove(m_pSCPICmdInfo->componentOrRpcName, this);
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


void VeinComponentScpiMeasureSequence::execute(ScpiModelTypes modelType, const ScpiTransactionId &scpiTransactionId)
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

int VeinComponentScpiMeasureSequence::entityID()
{
    return m_pSCPICmdInfo->entityId;
}

int VeinComponentScpiMeasureSequence::getInstanceCount()
{
    return m_instanceCount;
}

QString VeinComponentScpiMeasureSequence::convertVariantToString(const QVariant &value)
{
    if (value.canConvert<int>() || value.canConvert<double>())
        return QString("%1").arg(value.toDouble(), 0, 'g', 8);
    return value.toString();
}

QString VeinComponentScpiMeasureSequence::setAnswer(const QVariant &qvar)
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

void VeinComponentScpiMeasureSequence::measureInit()
{
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(measCont); // measure statemachine waits for measure value
}

void VeinComponentScpiMeasureSequence::measureFetch()
{
    emit sigMeasDone(m_sAnswer, m_measureScpiTransactionId, this);
    m_measureScpiTransactionId = ScpiTransactionId();
    emit measContinue();
}

void VeinComponentScpiMeasureSequence::configureDone()
{
    emit sigConfDone(m_configureScpiTransactionId, this);
}

void VeinComponentScpiMeasureSequence::readInit()
{
    // if not an init is still pending then
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the receiveMeasureValue slot, so we synchronized on next measurement value
    if (!m_bInitPending)
        VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(readCont); // read statemachine waits for measure value
}

void VeinComponentScpiMeasureSequence::readFetch()
{
    emit sigReadDone(m_sAnswer, m_readScpiTransactionId, this);
    m_readScpiTransactionId = ScpiTransactionId();
    emit readContinue();
}

void VeinComponentScpiMeasureSequence::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value

    m_bInitPending = true;
    VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->insert(m_pSCPICmdInfo->componentOrRpcName, this);

    signalList.append(initCont); // init statemachine waits for measure value
}

void VeinComponentScpiMeasureSequence::initDone()
{
    m_bInitPending = false;
    emit sigInitDone(m_initScpiTransactionId, this);
}

void VeinComponentScpiMeasureSequence::fetchSync()
{
    if (!m_bInitPending)
        emit fetchContinue();
    else
        signalList.append(fetchCont); // fetch statemachine waits for measure value
}

void VeinComponentScpiMeasureSequence::fetchFetch()
{
    emit sigFetchDone(m_sAnswer, m_fetchScpiTransactionId, this);
    m_fetchScpiTransactionId = ScpiTransactionId();
    emit fetchContinue();
}

}
