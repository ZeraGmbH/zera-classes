#include "test_sample_range_module_value_observer.h"
#include "rangemodulevalueobserver.h"
#include "demovaluesdsprange.h"
#include <vf-cpp-entity.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sample_range_module_value_observer)

void test_sample_range_module_value_observer::cleanup()
{
    m_testRunner.reset();
}

void test_sample_range_module_value_observer::fireSignalOnNewValuesTailoredRangeModule()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/session-minimal.json");
    createModule(RangeModuleEntityId, {{"SIG_Measuring", QVariant(1)}});

    RangeModuleValueObserver observer(m_testRunner->getVeinStorageSystem());
    QSignalSpy spy(&observer, &RangeModuleValueObserver::sigNewValues);

    m_testRunner->setVfComponent(RangeModuleEntityId, "SIG_Measuring", QVariant(0));
    QCOMPARE(spy.count(), 0);
    m_testRunner->setVfComponent(RangeModuleEntityId, "SIG_Measuring", QVariant(1));
    QCOMPARE(spy.count(), 1);
}

constexpr int phaseCountMt = 8;

void test_sample_range_module_value_observer::fireSignalOnNewValuesRealRangeModule()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");

    RangeModuleValueObserver observer(m_testRunner->getVeinStorageSystem());
    QSignalSpy spy(&observer, &RangeModuleValueObserver::sigNewValues);

    QCOMPARE(spy.count(), 0);
    fireRangeValues(42, phaseCountMt);
    QCOMPARE(spy.count(), 1);
}

void test_sample_range_module_value_observer::fireSignalTwiceOnSameValuesRealRangeModule()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");

    RangeModuleValueObserver observer(m_testRunner->getVeinStorageSystem());
    QSignalSpy spy(&observer, &RangeModuleValueObserver::sigNewValues);

    QCOMPARE(spy.count(), 0);
    fireRangeValues(42, phaseCountMt);
    fireRangeValues(42, phaseCountMt);
    QCOMPARE(spy.count(), 2);
}

constexpr float rangeUMt = 250;
constexpr float rangeIMt = 10;

void test_sample_range_module_value_observer::fireAndGetValidValuesMt()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    RangeModuleValueObserver observer(m_testRunner->getVeinStorageSystem());

    fireRangeValues(42, phaseCountMt);
    QCOMPARE(observer.getRelativeRangeValue("m0"), 42/rangeUMt);
    QCOMPARE(observer.getRelativeRangeValue("m3"), 42/rangeIMt);
    QCOMPARE(observer.getRelativeRangeValue("m6"), 42/rangeUMt);
    QCOMPARE(observer.getRelativeRangeValue("m7"), 0);

    fireRangeValues(37, phaseCountMt);
    QCOMPARE(observer.getRelativeRangeValue("m0"), 37/rangeUMt);
    QCOMPARE(observer.getRelativeRangeValue("m3"), 37/rangeIMt);
    QCOMPARE(observer.getRelativeRangeValue("m6"), 37/rangeUMt);
    QCOMPARE(observer.getRelativeRangeValue("m7"), 0);
}

void test_sample_range_module_value_observer::fireAndGetInValidValuesMt()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");

    RangeModuleValueObserver observer(m_testRunner->getVeinStorageSystem());
    fireRangeValues(42, phaseCountMt);

    QCOMPARE(observer.getRelativeRangeValue("m9"), float(qQNaN()));
    QCOMPARE(observer.getRelativeRangeValue("a0"), float(qQNaN()));
    QCOMPARE(observer.getRelativeRangeValue("foo"), float(qQNaN()));
    QCOMPARE(observer.getRelativeRangeValue("m0"), 42/rangeUMt);
}

void test_sample_range_module_value_observer::createModule(int entityId, QMap<QString, QVariant> components)
{
    VfCpp::VfCppEntity *entity = new VfCpp::VfCppEntity(entityId);
    m_testRunner->getModManFacade()->addSubsystem(entity);
    entity->initModule();
    for(const auto &componentName : components.keys())
        entity->createComponent(componentName, components[componentName]);
    TimeMachineObject::feedEventLoop();
}

void test_sample_range_module_value_observer::fireRangeValues(float rmsValue, int phaseCount)
{
    TestDspInterfacePtr dspInterface = m_testRunner->getDspInterface(RangeModuleEntityId, MODULEPROG);
    DemoValuesDspRange rangeValues(phaseCount);
    for(int i = 0; i < phaseCount; i++)
        rangeValues.setRmsPeakDCValue(i, rmsValue);
    dspInterface->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}
