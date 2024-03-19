#include "test_movingwindowfilter.h"
#include "movingwindowfilterwithoutsumfifo.h"
#include <timemachineobject.h>
#include <qtest.h>
#include <QSignalSpy>

QTEST_MAIN(test_movingwindowfilter)

void test_movingwindowfilter::init()
{
    if(!m_filteredValues.empty())
        m_filteredValues.clear();
}

void test_movingwindowfilter::oneSetOfActualValues()
{
    MovingWindowFilterWithoutSumFifo filter(1.0);
    TimeMachineObject::feedEventLoop();

    QVector<float> samples{1.0, 2.0, 3.0};
    connect(&filter, &MovingWindowFilterWithoutSumFifo::actualValues, this, &test_movingwindowfilter::receiveFilteredValues);
    filter.receiveActualValues(&samples);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(samples, m_filteredValues);
}

void test_movingwindowfilter::threeSetOfActualValues()
{
    MovingWindowFilterWithoutSumFifo filter(1.0);
    TimeMachineObject::feedEventLoop();

    QVector<float> samples1{1.0, 2.0, 3.0};
    QVector<float> samples2{1.1, 2.1, 3.1};
    QVector<float> samples3{1.2, 2.2, 3.2};
    QVector<float> averageSamples(samples1.count());

    connect(&filter, &MovingWindowFilterWithoutSumFifo::actualValues, this, &test_movingwindowfilter::receiveFilteredValues);

    filter.receiveActualValues(&samples1);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(samples1, m_filteredValues);

    filter.receiveActualValues(&samples2);
    TimeMachineObject::feedEventLoop();
    for(int i = 0; i < samples1.count(); i++)
        averageSamples[i] = (samples1[i] + samples2[i]) / 2;
    QCOMPARE(averageSamples, m_filteredValues);

    filter.receiveActualValues(&samples3);
    TimeMachineObject::feedEventLoop();
    for(int i = 0; i < samples1.count(); i++)
        averageSamples[i] = (samples1[i] + samples2[i] + samples3[i]) / 3;
    QCOMPARE(averageSamples, m_filteredValues);
}

void test_movingwindowfilter::receiveFilteredValues(QVector<float> *filteredValues)
{
    m_filteredValues.resize(filteredValues->size());
    for(int i = 0; i < filteredValues->size(); i++)
        m_filteredValues[i] = filteredValues->at(i);
}

