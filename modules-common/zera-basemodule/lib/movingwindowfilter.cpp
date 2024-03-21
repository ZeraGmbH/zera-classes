#include "movingwindowfilter.h"
#include <timerfactoryqt.h>

cMovingwindowFilter::cMovingwindowFilter(float time)
    :m_fintegrationTime(time)
{
    //setting up a statemachine for filtering
    m_pactiveState = new QState();
    m_pinitFilterState = new QState(m_pactiveState);
    m_psetupFilterState = new QState(m_pactiveState);
    m_pbuildupFilterState = new QState(m_pactiveState);
    m_preadyFilterState = new QState(m_pactiveState);
    m_pdoFilterState = new QState(m_pactiveState);
    m_pFinishState = new QFinalState();

    m_pactiveState->addTransition(this, &cMovingwindowFilter::finishFilter, m_pFinishState);
    m_pinitFilterState->addTransition(this, &cMovingwindowFilter::newActualValues, m_psetupFilterState);
    m_psetupFilterState->addTransition(this, &cMovingwindowFilter::timerInitialized, m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(this, &cMovingwindowFilter::newActualValues, m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(this, &cMovingwindowFilter::integrationTimeExpired, m_preadyFilterState);
    m_preadyFilterState->addTransition(this, &cMovingwindowFilter::newActualValues, m_pdoFilterState);
    m_pdoFilterState->addTransition(this, &cMovingwindowFilter::newActualValues, m_pdoFilterState);
    connect(&m_FilterStatemachine, &QStateMachine::stopped, this, &cMovingwindowFilter::restartFilter);

    m_FilterStatemachine.addState(m_pactiveState);
    m_FilterStatemachine.addState(m_pFinishState);
    m_FilterStatemachine.setInitialState(m_pactiveState);
    m_pactiveState->setInitialState(m_pinitFilterState);

    connect(m_pinitFilterState, &QState::entered, this, &cMovingwindowFilter::initFilter);
    connect(m_psetupFilterState, &QState::entered, this, &cMovingwindowFilter::setupFilter);
    connect(m_pbuildupFilterState, &QState::entered, this, &cMovingwindowFilter::buildupFilter);
    connect(m_pdoFilterState, &QState::entered, this, &cMovingwindowFilter::doFilter);
    connect(m_pFinishState, &QState::entered, this, &cMovingwindowFilter::stopFilter);

    m_FilterStatemachine.start();
}

void cMovingwindowFilter::receiveActualValues(QVector<float> *actualValues)
{
    m_pActualValues = actualValues;
    emit newActualValues();
}

void cMovingwindowFilter::setIntegrationtime(float time)
{
    m_fintegrationTime = time;
    emit finishFilter();
}

void cMovingwindowFilter::addnewValues()
{
    int n;
    QVector<double> newValues;

    n = m_pActualValues->count();
    for (int i = 0; i < n; i++) {
        newValues.append(m_pActualValues->at(i));
    }
    m_ActValueFifoList.append(newValues);
    int m = m_ActValueFifoList.count();
    for (int i = 0; i < newValues.count(); i++) {
        m_FifoSum.replace(i, m_FifoSum.at(i) + newValues.at(i));
        m_ActualValues.replace(i, m_FifoSum.at(i) / m); // our filtered actual values
    }
    emit actualValues(&m_ActualValues);
}

void cMovingwindowFilter::initFilter()
{
    m_ActValueFifoList.clear();
    m_integrationTimer = TimerFactoryQt::createSingleShot((int)(m_fintegrationTime * 1000.0));
    connect(m_integrationTimer.get(), &TimerTemplateQt::sigExpired, this, &cMovingwindowFilter::integrationTimeExpired);
}

void cMovingwindowFilter::setupFilter()
{
    m_integrationTimer->start(); // while timer is running we'll fill the fifo
    m_FifoSum.fill(0.0, m_pActualValues->size());
    m_ActualValues.resize(m_pActualValues->size());
    emit timerInitialized();
}

void cMovingwindowFilter::buildupFilter()
{
    addnewValues(); // we must add the new values while building up our filter
}

void cMovingwindowFilter::doFilter()
{
    QVector<double> removeValues = m_ActValueFifoList.at(0);
    int n = removeValues.count();
    for (int i = 0; i < n; i++) {
        m_FifoSum.replace(i, m_FifoSum.at(i) - removeValues.at(i)); // we remove the first value from our sum
    }
    m_ActValueFifoList.removeFirst(); // and remove all values from our fifo
    addnewValues(); // and then add the new ones
}

void cMovingwindowFilter::stopFilter()
{
    m_FilterStatemachine.stop(); // we restart at once
}

void cMovingwindowFilter::restartFilter()
{
    m_FilterStatemachine.start();
}

