#include "test_modman_config.h"
#include "modulemanagerconfigtest.h"
#include <QTest>

QTEST_MAIN(test_modman_config)

void test_modman_config::loadDevConfig()
{
    ModulemanagerConfigTest::enableTest();
    ModulemanagerConfig::setDemoDevice("mt310s2");

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QStringList availSessions = mmConfig->getAvailableSessions();

    QVERIFY(mmConfig->isValid());
    QCOMPARE(availSessions.count(), 6);
    QVERIFY(availSessions.contains("mt310s2-meas-session.json"));
    QVERIFY(availSessions.contains("mt310s2-dc-session.json"));
    QVERIFY(availSessions.contains("mt310s2-emob-session-ac.json"));
    QVERIFY(availSessions.contains("mt310s2-emob-session-dc.json"));
    QVERIFY(availSessions.contains("mt310s2-dc-session-dev.json"));
    QVERIFY(availSessions.contains("mt310s2-ced-session.json"));
    QCOMPARE(mmConfig->getDefaultSession(), "mt310s2-meas-session.json");
}
