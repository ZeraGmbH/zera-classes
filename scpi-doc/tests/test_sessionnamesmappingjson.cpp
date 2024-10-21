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
