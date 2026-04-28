#include "test_sourceswitchjsonangleadjustmentdecorator.h"
#include "sourceswitchjsonangleadjustmentdecorator.h"
#include "testsourceswitchjson.h"
#include <testloghelpers.h>
#include <vs_storagecomponent.h>
#include <QJsonDocument>
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
    SourceSwitchJsonAngleAdjustmentDecorator decorator(TestSourceSwitchJson::create(caps),
                                                       caps,
                                                       createVeinDftValuesEmpty(3),
                                                       createVeinDftValuesEmpty(3),
                                                       std::make_shared<VeinStorage::StorageComponent>(QVariant()));

}


QList<VeinStorage::AbstractComponentPtr> test_sourceswitchjsonangleadjustmentdecorator::createVeinDftValuesEmpty(int phaseCount)
{
    QList<VeinStorage::AbstractComponentPtr> values;
    for (int i=0; i<phaseCount; ++i)
        values.append(std::make_shared<VeinStorage::StorageComponent>(QVariant()));
    return values;
}
