#include "movingwindowfilter.h"

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

    m_pactiveState->addTransition(this, SIGNAL(finishFilter()), m_pFinishState);
    m_pinitFilterState->addTransition(this, SIGNAL(newActualValues()), m_psetupFilterState);
    m_psetupFilterState->addTransition(this, SIGNAL(timerInitialized()), m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(this, SIGNAL(newActualValues()), m_pbuildupFilterState);
    m_pbuildupFilterState->addTransition(&m_integrationTimer, SIGNAL(timeout()), m_preadyFilterState);
    m_preadyFilterState->addTransition(this, SIGNAL(newActualValues()), m_pdoFilterState);
    m_pdoFilterState->addTransition(this, SIGNAL(newActualValues()), m_pdoFilterState);
    connect(&m_FilterStatemachine, SIGNAL(stopped()), this, SLOT(restartFilter()));

    m_FilterStatemachine.addState(m_pactiveState);
    m_FilterStatemachine.addState(m_pFinishState);
    m_FilterStatemachine.setInitialState(m_pactiveState);
    m_pactiveState->setInitialState(m_pinitFilterState);

    connect(m_pinitFilterState, SIGNAL(entered()), SLOT(initFilter()));
    connect(m_psetupFilterState, SIGNAL(entered()), SLOT(setupFilter()));
    connect(m_pbuildupFilterState, SIGNAL(entered()), SLOT(buildupFilter()));
    connect(m_pdoFilterState, SIGNAL(entered()), SLOT(doFilter()));
    connect(m_pFinishState, SIGNAL(entered()), SLOT(stopFilter()));

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
    //QVector<float> newValues(*m_pActualValues);
    QVector<double> newValues;

    n = m_pActualValues->count();
    for (int i = 0; i < n; i++)
        newValues.append(m_pActualValues->at(i));

    m_ActValueFifoList.append(newValues); // we append the next values
    int m = m_ActValueFifoList.count();

    for (int i = 0; i < newValues.count(); i++)
    {
        m_FifoSum.replace(i, m_FifoSum.at(i) + newValues.at(i));
        m_ActualValues.replace(i, m_FifoSum.at(i) / m); // our filtered actual values
    }

    emit actualValues(&m_ActualValues); // we emit them here
}


void cMovingwindowFilter::initFilter()
{
    m_ActValueFifoList.clear(); // we clear actual fifo
    m_integrationTimer.stop();
    m_integrationTimer.setSingleShot(true);
}


void cMovingwindowFilter::setupFilter()
{
    m_integrationTimer.start((int)(m_fintegrationTime * 1000.0)); // while timer is running we'll fill the fifo
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

    for (int i = 0; i < n; i++)
    {
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



