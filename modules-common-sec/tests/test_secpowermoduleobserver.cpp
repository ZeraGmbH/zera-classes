#include "test_secpowermoduleobserver.h"
#include "secpowermoduleobserver.h"
#include "modulemanagertestrunner.h"
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_secpowermoduleobserver)

const int power1EntityId = 1073;
const int power2EntityId = 1090;

void test_secpowermoduleobserver::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_secpowermoduleobserver::init()
{
    if (m_testRunner == nullptr) {
        m_testRunner.reset();
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/session-minimal.json");
    }
}

void test_secpowermoduleobserver::signalOnEntityIdInitialSet()
{
    SecPowerModuleObserver observer(nullptr);
    QSignalSpy spy(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged);

    observer.setPowerModulentityId(10);
    QCOMPARE(spy.size(), 1);
}

void test_secpowermoduleobserver::signalOnEntityIdInitialSetTwiceSame()
{
    SecPowerModuleObserver observer(nullptr);
    QSignalSpy spy(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged);

    observer.setPowerModulentityId(10);
    observer.setPowerModulentityId(10);
    QCOMPARE(spy.size(), 1);

}

void test_secpowermoduleobserver::entityGetOnSignal()
{
    SecPowerModuleObserver observer(nullptr);
    int entityIdReceived = -1;
    connect(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged, this, [&]() {
        entityIdReceived = observer.getPowerModulentityId();
    });

    observer.setPowerModulentityId(10);
    QCOMPARE(entityIdReceived, 10);
}

void test_secpowermoduleobserver::changeEntity()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);
    QSignalSpy spy(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged);

    observer.setPowerModulentityId(power1EntityId);
    QCOMPARE(spy.size(), 1);
    QCOMPARE(observer.getPowerType(), "Q");

    connect(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged, this, [&]() {
        // are values set on notification?
        QCOMPARE(observer.getPowerType(), "P");
        QCOMPARE(observer.getConstant(), 96000000);
    });
    observer.setPowerModulentityId(power2EntityId);
}

void test_secpowermoduleobserver::initialPowerType()
{
    SecPowerModuleObserver observer(nullptr);
    QCOMPARE(observer.getPowerType(), "");
}

void test_secpowermoduleobserver::getPowertypePower1()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(power1EntityId);
    QCOMPARE(observer.getPowerType(), "Q");
}

void test_secpowermoduleobserver::getPowertypePower2()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(power2EntityId);
    QCOMPARE(observer.getPowerType(), "P");
}

void test_secpowermoduleobserver::getPowertypeInvalid()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(42);
    QCOMPARE(observer.getPowerType(), "");
}

void test_secpowermoduleobserver::initialConstant()
{
    SecPowerModuleObserver observer(nullptr);
    QCOMPARE(observer.getConstant(), 0.0);
}

void test_secpowermoduleobserver::getConstantPower1()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(power1EntityId);
    QCOMPARE(observer.getConstant(), 288000000);
}

void test_secpowermoduleobserver::getConstantPower2()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(power2EntityId);
    QCOMPARE(observer.getConstant(), 96000000);
}

void test_secpowermoduleobserver::getConstantPowerInvalid()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);

    observer.setPowerModulentityId(42);
    QCOMPARE(observer.getConstant(), 0.0);
}

void test_secpowermoduleobserver::changeMeasMode()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);
    observer.setPowerModulentityId(power1EntityId);

    QCOMPARE(m_testRunner->getVfComponent(power1EntityId, "PAR_MeasuringMode"), "2LB");
    QCOMPARE(observer.getPowerType(), "Q");
    QCOMPARE(observer.getConstant(), 288000000);
    QSignalSpy spy(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged);

    QString finalPowerType;
    double finalConstant = 0.0;
    connect(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged, this, [&]() {
        // are values set on final notification?
        finalPowerType = observer.getPowerType();
        finalConstant = observer.getConstant();
    });
    m_testRunner->setVfComponent(power1EntityId, "PAR_MeasuringMode", "4LW");
    QCOMPARE(m_testRunner->getVfComponent(power1EntityId, "PAR_MeasuringMode"), "4LW");
    QCOMPARE(finalPowerType, "P");
    QCOMPARE(finalConstant, 96000000);
    QCOMPARE(spy.size(), 2); // one for powertype / one for constant

    m_testRunner.reset(); // server state change -> avoid surprise
}

void test_secpowermoduleobserver::changeEntityIdThenMeasMode()
{
    VeinStorage::AbstractDatabase *storageDb = m_testRunner->getVeinStorageDb();
    SecPowerModuleObserver observer(storageDb);
    observer.setPowerModulentityId(power2EntityId);
    QSignalSpy spy(&observer, &SecPowerModuleObserver::sigPowerModuleValuesChanged);

    observer.setPowerModulentityId(power1EntityId);
    QCOMPARE(spy.size(), 1);

    QCOMPARE(m_testRunner->getVfComponent(power1EntityId, "PAR_MeasuringMode"), "2LB");
    m_testRunner->setVfComponent(power1EntityId, "PAR_MeasuringMode", "4LW");
    QCOMPARE(m_testRunner->getVfComponent(power1EntityId, "PAR_MeasuringMode"), "4LW");

    QCOMPARE(spy.size(), 3); // above / powertype / constant
    m_testRunner.reset(); // server state change -> avoid surprise
}
