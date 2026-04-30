#include "test_sourceswitchjsonangleadjustmentdecorator.h"
#include "sourceswitchjsonangleadjustmentdecorator.h"
#include "testsourceswitchjson.h"
#include <testloghelpers.h>
#include <vs_storagecomponent.h>
#include <timemachineobject.h>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sourceswitchjsonangleadjustmentdecorator)

void test_sourceswitchjsonangleadjustmentdecorator::initialLoadpoint()
{
    QJsonObject caps = QJsonDocument::fromJson(TestLogHelpers::loadFile(":/capabilities/mt581s2_valid.json")).object();
    SourceSwitchJsonAngleAdjustmentDecorator decorator(TestSourceSwitchJson::create(caps),
                                                       caps,
                                                       createVeinDftValuesEmpty(3),
                                                       createVeinDftValuesEmpty(3),
                                                       std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    QByteArray dumped = TestLogHelpers::dump(decorator.getCurrLoadpoint().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/loadDumps/initialLoad.json", dumped));
}

void test_sourceswitchjsonangleadjustmentdecorator::switchOn()
{
    QJsonObject caps = QJsonDocument::fromJson(TestLogHelpers::loadFile(":/capabilities/mt581s2_valid.json")).object();
    std::shared_ptr<TestSourceSwitchJson> switcherToAdjust = TestSourceSwitchJson::create(caps);
    SourceSwitchJsonAngleAdjustmentDecorator decorator(switcherToAdjust,
                                                       caps,
                                                       createVeinDftValuesEmpty(3),
                                                       createVeinDftValuesEmpty(3),
                                                       std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    QSignalSpy spy(&decorator, &AbstractSourceSwitchJson::sigSwitchFinished);

    JsonParamApi load = decorator.getCurrLoadpoint();
    load.setOn(true);

    int switchOnId = decorator.switchState(load);
    switcherToAdjust->fireFinish(true);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
    QCOMPARE(spy[0][1], switchOnId);

    QByteArray dumpedCurrent = TestLogHelpers::dump(decorator.getCurrLoadpoint().getParams());
    QByteArray expectedCurrent = TestLogHelpers::dump(load.getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expectedCurrent, dumpedCurrent));
}

void test_sourceswitchjsonangleadjustmentdecorator::switchOnOffOk()
{
    QJsonObject caps = QJsonDocument::fromJson(TestLogHelpers::loadFile(":/capabilities/mt581s2_valid.json")).object();
    std::shared_ptr<TestSourceSwitchJson> switcherToAdjust = TestSourceSwitchJson::create(caps);
    SourceSwitchJsonAngleAdjustmentDecorator decorator(switcherToAdjust,
                                                       caps,
                                                       createVeinDftValuesEmpty(3),
                                                       createVeinDftValuesEmpty(3),
                                                       std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    QSignalSpy spy(&decorator, &AbstractSourceSwitchJson::sigSwitchFinished);

    JsonParamApi load = decorator.getCurrLoadpoint();

    load.setOn(true);
    int switchOnId = decorator.switchState(load);
    switcherToAdjust->fireFinish(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], true);
    QCOMPARE(spy[0][1], switchOnId);

    load.setOn(false);
    int switchOffId = decorator.switchState(load);
    switcherToAdjust->fireFinish(true);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[1][0], true);
    QCOMPARE(spy[1][1], switchOffId);

    QByteArray dumpedCurrent = TestLogHelpers::dump(decorator.getCurrLoadpoint().getParams());
    QByteArray expectedCurrent = TestLogHelpers::dump(load.getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expectedCurrent, dumpedCurrent));
}

void test_sourceswitchjsonangleadjustmentdecorator::switchOnError()
{
    QJsonObject caps = QJsonDocument::fromJson(TestLogHelpers::loadFile(":/capabilities/mt581s2_valid.json")).object();
    std::shared_ptr<TestSourceSwitchJson> switcherToAdjust = TestSourceSwitchJson::create(caps);
    SourceSwitchJsonAngleAdjustmentDecorator decorator(switcherToAdjust,
                                                       caps,
                                                       createVeinDftValuesEmpty(3),
                                                       createVeinDftValuesEmpty(3),
                                                       std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    QSignalSpy spy(&decorator, &AbstractSourceSwitchJson::sigSwitchFinished);

    JsonParamApi load = decorator.getCurrLoadpoint();
    load.setOn(true);

    int switchOnId = decorator.switchState(load);
    switcherToAdjust->fireFinish(false);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], false);
    QCOMPARE(spy[0][1], switchOnId);

    QByteArray dumpedCurrent = TestLogHelpers::dump(decorator.getCurrLoadpoint().getParams());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/loadDumps/initialLoad.json", dumpedCurrent));
}

QList<VeinStorage::AbstractComponentPtr> test_sourceswitchjsonangleadjustmentdecorator::createVeinDftValuesEmpty(int phaseCount)
{
    QList<VeinStorage::AbstractComponentPtr> values;
    for (int i=0; i<phaseCount; ++i)
        values.append(std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    return values;
}
