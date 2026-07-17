#include "scpiveincomponentsequencemeasure.h"
#include <scpi.h>
#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <QVariant>
#include <QSequentialIterable>

namespace SCPIMODULE
{

int ScpiVeinComponentSequenceMeasure::m_instanceCount = 0;

ScpiVeinComponentSequenceMeasure::ScpiVeinComponentSequenceMeasure(const Params &params) :
    m_params(params)
{
    initialize();
    m_instanceCount++;
}

void ScpiVeinComponentSequenceMeasure::initialize()
{
    m_bInitPending = false;

    m_measureInitState.addTransition(this, &ScpiVeinComponentSequenceMeasure::measContinue, &m_measureFetchState);
    m_measureFetchState.addTransition(this, &ScpiVeinComponentSequenceMeasure::measContinue, &m_measureDoneState);
    m_MeasureStateMachine.addState(&m_measureInitState);
    m_MeasureStateMachine.addState(&m_measureFetchState);
    m_MeasureStateMachine.addState(&m_measureDoneState);
    connect(&m_measureInitState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::measureInit);
    connect(&m_measureFetchState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::measureFetch);
    m_MeasureStateMachine.setInitialState(&m_measureInitState);

    m_ConfigureStateMachine.addState(&m_confConfigureDoneState);
    connect(&m_confConfigureDoneState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::configureDone);
    m_ConfigureStateMachine.setInitialState(&m_confConfigureDoneState);

    m_readInitState.addTransition(this, &ScpiVeinComponentSequenceMeasure::readContinue, &m_readFetchState);
    m_readFetchState.addTransition(this, &ScpiVeinComponentSequenceMeasure::readContinue, &m_readDoneState);
    m_ReadStateMachine.addState(&m_readInitState);
    m_ReadStateMachine.addState(&m_readFetchState);
    m_ReadStateMachine.addState(&m_readDoneState);
    connect(&m_readInitState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::readInit);
    connect(&m_readFetchState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::readFetch);
    m_ReadStateMachine.setInitialState(&m_readInitState);

    m_initInitState.addTransition(this, &ScpiVeinComponentSequenceMeasure::initContinue, &m_initDoneState);
    m_InitStateMachine.addState(&m_initInitState);
    m_InitStateMachine.addState(&m_initDoneState);
    connect(&m_initInitState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::init);
    connect(&m_initDoneState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::initDone);
    m_InitStateMachine.setInitialState(&m_initInitState);

    m_fetchSyncState.addTransition(this, &ScpiVeinComponentSequenceMeasure::fetchContinue, &m_fetchFetchState);
    m_fetchFetchState.addTransition(this, &ScpiVeinComponentSequenceMeasure::fetchContinue, &m_fetchDoneState);
    m_FetchStateMachine.addState(&m_fetchSyncState);
    m_FetchStateMachine.addState(&m_fetchFetchState);
    m_FetchStateMachine.addState(&m_fetchDoneState);
    connect(&m_fetchSyncState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::fetchSync);
    connect(&m_fetchFetchState, &QState::entered, this, &ScpiVeinComponentSequenceMeasure::fetchFetch);
    m_FetchStateMachine.setInitialState(&m_fetchSyncState);
}

ScpiVeinComponentSequenceMeasure::~ScpiVeinComponentSequenceMeasure()
{
    m_instanceCount--;
}

enum signalCode {measCont, readCont, initCont, fetchCont};

void ScpiVeinComponentSequenceMeasure::receiveMeasureValue(const QVariant &value)
{
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


void ScpiVeinComponentSequenceMeasure::execute(ScpiModelTypes modelType, const ScpiTransactionId &scpiTransactionId)
{
    switch (modelType)
    {
    case ScpiModelTypes::MEASURE:
        if (!m_MeasureStateMachine.isRunning()) {
            m_measureScpiTransactionId = scpiTransactionId;
            m_MeasureStateMachine.start();
        }
        else
            qWarning("Measure state machine already running!");
        break;

    case ScpiModelTypes::CONFIGURE:
        if (!m_ConfigureStateMachine.isRunning()) {
            m_configureScpiTransactionId = scpiTransactionId;
            m_ConfigureStateMachine.start();
        }
        else
            qWarning("Configure state machine already running!");
        break;

    case ScpiModelTypes::READ:
        if (!m_ReadStateMachine.isRunning()) {
            m_readScpiTransactionId = scpiTransactionId;
            m_ReadStateMachine.start();
        }
        else
            qWarning("Model state machine already running!");
        break;

    case ScpiModelTypes::INIT:
        if (!m_InitStateMachine.isRunning()) {
            m_initScpiTransactionId = scpiTransactionId;
            m_InitStateMachine.start();
        }
        else
            qWarning("Init state machine already running!");
        break;

    case ScpiModelTypes::FETCH:
        if (!m_FetchStateMachine.isRunning()) {
            m_fetchScpiTransactionId = scpiTransactionId;
            m_FetchStateMachine.start();
        }
        else
            qWarning("Fetch state machine already running!");
        break;
    }
}

int ScpiVeinComponentSequenceMeasure::getInstanceCount()
{
    return m_instanceCount;
}

QString ScpiVeinComponentSequenceMeasure::convertVariantToString(const QVariant &value)
{
    if (value.canConvert<int>() || value.canConvert<double>())
        return QString("%1").arg(value.toDouble(), 0, 'g', 8);
    return value.toString();
}

QString ScpiVeinComponentSequenceMeasure::setAnswer(const QVariant &qvar)
{
    QString s;
    const QString unit = m_params.veinComponentInfo["Unit"].toString();
    bool isString = QMetaType::type(qvar.typeName()) == QMetaType::type("QString");
    if (!isString && qvar.canConvert<QVariantList>()) {
        QSequentialIterable iterable = qvar.value<QSequentialIterable>();
        s = QString("%1:%2:[%3]:")
                .arg(m_params.scpiModuleName, m_params.scpiCommand, unit);
        for (const QVariant &v : iterable)
            s += (convertVariantToString(v) + ",");
        s = s.remove(s.size()-1, 1);
    }
    else
        s = QString("%1:%2:[%3]:%4").arg(m_params.scpiModuleName,
                                         m_params.scpiCommand,
                                         unit,
                                         convertVariantToString(qvar));
    return s;
}

void ScpiVeinComponentSequenceMeasure::measureInit()
{
    signalList.append(measCont); // measure statemachine waits for measure value
}

void ScpiVeinComponentSequenceMeasure::measureFetch()
{
    emit sigMeasDone(m_sAnswer, m_measureScpiTransactionId, this);
    m_measureScpiTransactionId = ScpiTransactionId();
    emit measContinue();
}

void ScpiVeinComponentSequenceMeasure::configureDone()
{
    emit sigConfDone(m_configureScpiTransactionId, this);
}

void ScpiVeinComponentSequenceMeasure::readInit()
{
    signalList.append(readCont); // read statemachine waits for measure value
}

void ScpiVeinComponentSequenceMeasure::readFetch()
{
    emit sigReadDone(m_sAnswer, m_readScpiTransactionId, this);
    m_readScpiTransactionId = ScpiTransactionId();
    emit readContinue();
}

void ScpiVeinComponentSequenceMeasure::init()
{
    // we insert this object into the list of pending measurement values
    // the module's eventsystem will look for notifications on this and will
    // then call the initDone slot, so we synchronized on next measurement value

    m_bInitPending = true;

    signalList.append(initCont); // init statemachine waits for measure value
}

void ScpiVeinComponentSequenceMeasure::initDone()
{
    m_bInitPending = false;
    emit sigInitDone(m_initScpiTransactionId, this);
}

void ScpiVeinComponentSequenceMeasure::fetchSync()
{
    if (!m_bInitPending)
        emit fetchContinue();
    else
        signalList.append(fetchCont); // fetch statemachine waits for measure value
}

void ScpiVeinComponentSequenceMeasure::fetchFetch()
{
    emit sigFetchDone(m_sAnswer, m_fetchScpiTransactionId, this);
    m_fetchScpiTransactionId = ScpiTransactionId();
    emit fetchContinue();
}

}
