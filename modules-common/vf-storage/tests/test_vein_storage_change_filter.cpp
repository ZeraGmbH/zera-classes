#include "test_vein_storage_change_filter.h"
#include "veinstoragechangefilter.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_vein_storage_change_filter)

static constexpr int entityId1 = 10;
static constexpr int entityId2 = 11;
static const char* componentName1 = "ComponentName1";
static const char* componentName2 = "ComponentName2";
static constexpr int startTimeStamp = 0;

void test_vein_storage_change_filter::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_vein_storage_change_filter::init()
{
    TimeMachineForTest::reset();
    setupServer();
}

void test_vein_storage_change_filter::cleanup()
{
    m_server = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_vein_storage_change_filter::fireInitialValid()
{
    TimeMachineForTest::getInstance()->processTimers(42);
    m_server->setComponent(entityId1, componentName1, "foo");

    VeinStorageChangeFilter filter(m_server->getStorage(), true);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], entityId1);
    QCOMPARE(spy[0][1], componentName1);
    QCOMPARE(spy[0][2], "foo");
    QCOMPARE(spy[0][3], msAfterEpoch(42));
}

void test_vein_storage_change_filter::noFireInitialInvalid()
{
    VeinStorageChangeFilter filter(m_server->getStorage(), true);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);

    QCOMPARE(spy.count(), 0);
}

void test_vein_storage_change_filter::nofireInitialValid()
{
    m_server->setComponent(entityId1, componentName1, "foo");

    VeinStorageChangeFilter filter(m_server->getStorage(), false);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);

    QCOMPARE(spy.count(), 0);
}

void test_vein_storage_change_filter::nofireInitialValidWrongFilter()
{
    m_server->setComponent(entityId1, componentName1, "foo");

    VeinStorageChangeFilter filter(m_server->getStorage(), true);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(42, componentName1), false);
    QCOMPARE(filter.add(entityId1, "foo") , false);

    QCOMPARE(spy.count(), 0);
}

void test_vein_storage_change_filter::fireIntitialOnceOnTwoIdenticalValid()
{
    m_server->setComponent(entityId1, componentName1, "foo");

    VeinStorageChangeFilter filter(m_server->getStorage(), true);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);
    QCOMPARE(filter.add(entityId1, componentName1), false);

    QCOMPARE(spy.count(), 1);
}

void test_vein_storage_change_filter::fireInitialValidOpenClose()
{
    m_server->setComponent(entityId1, componentName1, "foo");

    VeinStorageChangeFilter filter(m_server->getStorage(), true);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);
    filter.clear();
    QCOMPARE(filter.add(entityId1, componentName1), true);
    QCOMPARE(spy.count(), 2);
}

void test_vein_storage_change_filter::fireChangeValid()
{
    VeinStorageChangeFilter filter(m_server->getStorage(), false);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);

    TimeMachineForTest::getInstance()->processTimers(42);
    m_server->setComponent(entityId1, componentName1, "foo");
    m_server->setComponent(entityId1, componentName2, "bar");
    m_server->setComponent(entityId2, componentName1, "bar");
    m_server->setComponent(entityId2, componentName2, "bar");

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], entityId1);
    QCOMPARE(spy[0][1], componentName1);
    QCOMPARE(spy[0][2], "foo");
    QCOMPARE(spy[0][3], msAfterEpoch(42));
}

void test_vein_storage_change_filter::fireChangeValidMultiple()
{
    VeinStorageChangeFilter filter(m_server->getStorage(), false);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);
    QCOMPARE(filter.add(entityId1, componentName2), true);
    QCOMPARE(filter.add(entityId2, componentName1), true);
    QCOMPARE(filter.add(entityId2, componentName2), true);

    m_server->setComponent(entityId1, componentName1, "foo");
    m_server->setComponent(entityId1, componentName2, "bar");
    m_server->setComponent(entityId2, componentName1, "baz");
    m_server->setComponent(entityId2, componentName2, "boz");

    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy[0][2], "foo");
    QCOMPARE(spy[1][2], "bar");
    QCOMPARE(spy[2][2], "baz");
    QCOMPARE(spy[3][2], "boz");
}

void test_vein_storage_change_filter::nofireChangeValidWrongFilter()
{
    VeinStorageChangeFilter filter(m_server->getStorage(), false);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(42, componentName1), false);
    QCOMPARE(filter.add(entityId1, "foo"), false);

    m_server->setComponent(entityId1, componentName1, "foo");

    QCOMPARE(spy.count(), 0);
}

void test_vein_storage_change_filter::fireChangeOnceOnTwoIdenticalValid()
{
    VeinStorageChangeFilter filter(m_server->getStorage(), false);
    QSignalSpy spy(&filter, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter.add(entityId1, componentName1), true);
    QCOMPARE(filter.add(entityId1, componentName1), false);

    m_server->setComponent(entityId1, componentName1, "foo");

    QCOMPARE(spy.count(), 1);
}

void test_vein_storage_change_filter::fireChangeTwoSequentialClearFilter()
{
    VeinStorageChangeFilter filter1(m_server->getStorage(), false);
    QSignalSpy spy1(&filter1, &VeinStorageChangeFilter::sigComponentValue);
    VeinStorageChangeFilter filter2(m_server->getStorage(), false);
    QSignalSpy spy2(&filter2, &VeinStorageChangeFilter::sigComponentValue);

    QCOMPARE(filter1.add(entityId1, componentName1), true);
    QCOMPARE(filter2.add(entityId1, componentName1), true);

    m_server->setComponent(entityId1, componentName1, "foo");
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    spy1.clear();
    spy2.clear();
    filter1.clear();
    m_server->setComponent(entityId1, componentName1, "bar");
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

    spy1.clear();
    spy2.clear();
    filter2.clear();
    m_server->setComponent(entityId1, componentName1, "baz");
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
}

void test_vein_storage_change_filter::setupServer()
{
    m_server = std::make_unique<TestVeinServer>();

    m_server->addTestEntities(3, 3);
    TimeMachineObject::feedEventLoop();

    m_server->simulAllModulesLoaded("test-session1.json", QStringList() << "test-session1.json" << "test-session2.json");
}

QDateTime test_vein_storage_change_filter::msAfterEpoch(qint64 msecs)
{
    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(msecs);
    return dateTime;
}
