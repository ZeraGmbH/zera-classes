#include "test_sessionnamesmappingjson.h"
#include "sessionnamesmappingjson.h"
#include "modulemanagerconfigtest.h"
#include <QJsonArray>
#include <QTest>

QTEST_MAIN(test_sessionnamesmappingjson)

void test_sessionnamesmappingjson::testMt310s2Com5003SessionCount()
{
    ModulemanagerConfigTest::supportOeTests();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());

    QCOMPARE(sessionNamesMapping.getSessionCount("mt310s2"), mmConfig->getAvailableSessions().count());
    ModulemanagerConfig::setDemoDevice("com5003", false);
    QCOMPARE(sessionNamesMapping.getSessionCount("com5003"), mmConfig->getAvailableSessions().count());
    QCOMPARE(sessionNamesMapping.getSessionCount("foo"), 0);
}

void test_sessionnamesmappingjson::testSessionNames()
{
    ModulemanagerConfigTest::supportOeTests();
    ModulemanagerConfig::setDemoDevice("mt310s2", false);
    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    SessionNamesMappingJson sessionNamesMapping(ModulemanagerConfig::getConfigFileNameFull());

    QCOMPARE(sessionNamesMapping.getSessionNameForExternalUsers("mt310s2-meas-session.json"), "Default");
    QCOMPARE(sessionNamesMapping.getSessionNameForExternalUsers("mt310s2-dc-session.json"), "DC: 4*Voltage / 1*Current");
    QCOMPARE(sessionNamesMapping.getSessionNameForExternalUsers("com5003-ced-session.json"), "Changing energy direction");
    QCOMPARE(sessionNamesMapping.getSessionNameForExternalUsers("com5003-perphase-session.json"), "3 Systems / 2 Wires");
    QCOMPARE(sessionNamesMapping.getSessionNameForExternalUsers("foo"), "");
}
