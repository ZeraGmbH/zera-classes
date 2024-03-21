#include "test_movingwindowfilter.h"
#include "movingwindowfilterwithoutsumfifo.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <qtest.h>
#include <QSignalSpy>

QTEST_MAIN(test_movingwindowfilter)

void test_movingwindowfilter::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_movingwindowfilter::cleanup()
{
    m_filteredValues.clear();
}

void test_movingwindowfilter::threeSetOfActualValuesWithoutIntegrationTimeElapsed()
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
        //cast 'samples' into 'double' to match with steps & accuracy from MovingWindowFilterWithoutSumFifo calculations
        averageSamples[i] = (static_cast<double>(samples1[i]) + static_cast<double>(samples2[i])) / 2;
    QCOMPARE(averageSamples, m_filteredValues);

    filter.receiveActualValues(&samples3);
    TimeMachineObject::feedEventLoop();
    for(int i = 0; i < samples1.count(); i++)
        averageSamples[i] = (static_cast<double>(samples1[i]) + static_cast<double>(samples2[i]) + static_cast<double>(samples3[i])) / 3;
    QCOMPARE(averageSamples, m_filteredValues);
}

void test_movingwindowfilter::threeSetOfActualValuesWithIntegrationTimeElapsed()
{
    MovingWindowFilterWithoutSumFifo filter(1.0);
    TimeMachineObject::feedEventLoop();

    QVector<float> samples1{1.0, 2.0, 3.0};
    QVector<float> samples2{4.0, 5.0, 6.0};
    QVector<float> samples3{7.0, 8.0, 9.0};
    QVector<float> averageSamples(samples1.count());

    connect(&filter, &MovingWindowFilterWithoutSumFifo::actualValues, this, &test_movingwindowfilter::receiveFilteredValues);

    filter.receiveActualValues(&samples1);
    TimeMachineObject::feedEventLoop();
    filter.receiveActualValues(&samples2);
    TimeMachineObject::feedEventLoop();

    for(int i = 0; i < samples1.count(); i++)
        //cast 'samples' into 'double' to match with steps & accuracy from MovingWindowFilterWithoutSumFifo calculations
        averageSamples[i] = (static_cast<double>(samples1[i]) + static_cast<double>(samples2[i])) / 2;
    QCOMPARE(averageSamples, m_filteredValues);

    TimeMachineForTest::getInstance()->processTimers(1000);
    filter.receiveActualValues(&samples3);
    TimeMachineObject::feedEventLoop();
    for(int i = 0; i < samples1.count(); i++)
        averageSamples[i] = (static_cast<double>(samples2[i]) + static_cast<double>(samples3[i])) / 2;
    QCOMPARE(averageSamples, m_filteredValues);
}

void test_movingwindowfilter::receiveFilteredValues(QVector<float> *filteredValues)
{
    m_filteredValues = *filteredValues;
}
