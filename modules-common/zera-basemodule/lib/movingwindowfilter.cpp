#include "movingwindowfilter.h"
#include <timerfactoryqt.h>

MovingwindowFilter::MovingwindowFilter(float time)
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

    m_pactiveState->addTransition(this, &MovingwindowFilter::sigFinishFilter, m_pFinishState);
    m_pinitFilterState->addTransition(this, &MovingwindowFilter::sigNewActualValues, m_psetupFilterState);
    m_psetupFilterState->addTransition(this, &MovingwindowFilter::sigTimerInitialized, m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(this, &MovingwindowFilter::sigNewActualValues, m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(this, &MovingwindowFilter::sigIntegrationTimeExpired, m_preadyFilterState);
    m_preadyFilterState->addTransition(this, &MovingwindowFilter::sigNewActualValues, m_pdoFilterState);
    m_pdoFilterState->addTransition(this, &MovingwindowFilter::sigNewActualValues, m_pdoFilterState);
    connect(&m_FilterStatemachine, &QStateMachine::stopped, this, &MovingwindowFilter::restartFilter);

    m_FilterStatemachine.addState(m_pactiveState);
    m_FilterStatemachine.addState(m_pFinishState);
    m_FilterStatemachine.setInitialState(m_pactiveState);
    m_pactiveState->setInitialState(m_pinitFilterState);

    connect(m_pinitFilterState, &QState::entered, this, &MovingwindowFilter::initFilter);
    connect(m_psetupFilterState, &QState::entered, this, &MovingwindowFilter::setupFilter);
    connect(m_pbuildupFilterState, &QState::entered, this, &MovingwindowFilter::buildupFilter);
    connect(m_pdoFilterState, &QState::entered, this, &MovingwindowFilter::doFilter);
    connect(m_pFinishState, &QState::entered, this, &MovingwindowFilter::stopFilter);

    m_FilterStatemachine.start();
}

void MovingwindowFilter::receiveActualValues(QVector<float> *actualValues)
{
    m_pActualValues = actualValues;
    emit sigNewActualValues();
}

void MovingwindowFilter::setIntegrationTime(float time)
{
    m_fintegrationTime = time;
    emit sigFinishFilter();
}

void MovingwindowFilter::addnewValues()
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
    emit sigActualValues(&m_ActualValues);
}

void MovingwindowFilter::initFilter()
{
    m_ActValueFifoList.clear();
    m_integrationTimer = TimerFactoryQt::createSingleShot((int)(m_fintegrationTime * 1000.0));
    connect(m_integrationTimer.get(), &TimerTemplateQt::sigExpired, this, &MovingwindowFilter::sigIntegrationTimeExpired);
}

void MovingwindowFilter::setupFilter()
{
    m_integrationTimer->start(); // while timer is running we'll fill the fifo
    m_FifoSum.fill(0.0, m_pActualValues->size());
    m_ActualValues.resize(m_pActualValues->size());
    emit sigTimerInitialized();
}

void MovingwindowFilter::buildupFilter()
{
    addnewValues(); // we must add the new values while building up our filter
}

void MovingwindowFilter::doFilter()
{
    QVector<double> removeValues = m_ActValueFifoList.at(0);
    int n = removeValues.count();
    for (int i = 0; i < n; i++) {
        m_FifoSum.replace(i, m_FifoSum.at(i) - removeValues.at(i)); // we remove the first value from our sum
    }
    m_ActValueFifoList.removeFirst(); // and remove all values from our fifo
    addnewValues(); // and then add the new ones
}

void MovingwindowFilter::stopFilter()
{
    m_FilterStatemachine.stop(); // we restart at once
}

void MovingwindowFilter::restartFilter()
{
    m_FilterStatemachine.start();
}

