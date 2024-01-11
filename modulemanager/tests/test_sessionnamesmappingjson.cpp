#include "test_sessionnamesmappingjson.h"
#include "sessionnamesmappingjson.h"
#include "modulemanagerconfigtest.h"
#include <QJsonArray>
#include <QTest>

QTEST_MAIN(test_sessionnamesmappingjson)

void test_sessionnamesmappingjson::testMt310s2Sessions()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QJsonObject jsonData = SessionNamesMappingJson::createSessionNamesMappingJson(ModulemanagerConfig::getConfigFileNameFull(), "mt310s2");
    QVERIFY(jsonData.contains("mt310s2"));
    QCOMPARE(jsonData["mt310s2"].toObject().count(), mmConfig->getAvailableSessions().count());
}

void test_sessionnamesmappingjson::testMt310s2Com5003Sessions()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QJsonObject jsonData = SessionNamesMappingJson::createSessionNamesMappingJsonAllDevices(ModulemanagerConfig::getConfigFileNameFull());

    QVERIFY(jsonData.contains("mt310s2"));
    QCOMPARE(jsonData["mt310s2"].toObject().count(), mmConfig->getAvailableSessions().count());
    ModulemanagerConfig::setDemoDevice("com5003", false);
    QVERIFY(jsonData.contains("com5003"));
    QCOMPARE(jsonData["com5003"].toObject().count(), mmConfig->getAvailableSessions().count());
}

