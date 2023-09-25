#include "test_modman_config.h"
#include "modulemanagerconfigtest.h"
#include <QTest>

QTEST_MAIN(test_modman_config)

void test_modman_config::loadDevConfig()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("demo");

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QStringList availSessions = mmConfig->getAvailableSessions();

    QVERIFY(mmConfig->isValid());
    QCOMPARE(availSessions.count(), 2);
    QVERIFY(availSessions.contains("demo-session.json"));
    QVERIFY(availSessions.contains("demo-session1.json"));
    QCOMPARE(mmConfig->getDefaultSession(), "demo-session.json");
}
