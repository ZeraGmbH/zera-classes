#include "test_modman_config.h"
#include "modulemanagerconfigtest.h"
#include "qjsonarray.h"
#include <zera-jsonfileloader.h>
#include <QTest>

QTEST_MAIN(test_modman_config)

void test_modman_config::loadDevConfig()
{
    ModulemanagerConfigTest::supportOeTests();
    ModulemanagerConfig::setDemoDevice("mt310s2", true);

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    QStringList availSessions = mmConfig->getAvailableSessions();

    QVERIFY(mmConfig->isValid());
    QCOMPARE(availSessions.count(), 5);
    QVERIFY(availSessions.contains("mt310s2-meas-session.json"));
    QVERIFY(availSessions.contains("mt310s2-dc-session.json"));
    QVERIFY(availSessions.contains("mt310s2-emob-session-ac.json"));
    QVERIFY(availSessions.contains("mt310s2-emob-session-dc.json"));
    QVERIFY(availSessions.contains("mt310s2-ced-session.json"));
    QCOMPARE(mmConfig->getDefaultSession(), "mt310s2-meas-session.json");
}

void test_modman_config::verifySessionsCount()
{
    ModulemanagerConfigTest::supportOeTests();
    QString configDirName = QDir::cleanPath(QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONFIG_PATH));

    QString configFile = configDirName + "/" + MODMAN_DEFAULT_SESSION;
    QJsonObject jsonConfig = cJsonFileLoader::loadJsonFile(configFile).value("mt310s2").toObject();
    QCOMPARE(jsonConfig.value("availableSessions").toArray().count(), jsonConfig.value("sessionDisplayStrings").toArray().count());
    jsonConfig = cJsonFileLoader::loadJsonFile(configFile).value("com5003").toObject();
    QCOMPARE(jsonConfig.value("availableSessions").toArray().count(), jsonConfig.value("sessionDisplayStrings").toArray().count());

    configFile = configDirName + "/" + MODMAN_TEST_SESSION;
    jsonConfig = cJsonFileLoader::loadJsonFile(configFile).value("mt310s2").toObject();
    QCOMPARE(jsonConfig.value("availableSessions").toArray().count(), jsonConfig.value("sessionDisplayStrings").toArray().count());
    jsonConfig = cJsonFileLoader::loadJsonFile(configFile).value("com5003").toObject();
    QCOMPARE(jsonConfig.value("availableSessions").toArray().count(), jsonConfig.value("sessionDisplayStrings").toArray().count());
}

void test_modman_config::pcbServiceConnectionInfo()
{
    ModulemanagerConfigTest::supportOeTests();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    NetworkConnectionInfo info = mmConfig->getPcbConnectionInfo();
    QCOMPARE(info.m_sIP, getConfiguredIp());
    QCOMPARE(info.m_nPort, 6307);
}

void test_modman_config::dspServiceConnectionInfo()
{
    ModulemanagerConfigTest::supportOeTests();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    NetworkConnectionInfo info = mmConfig->getDspConnectionInfo();
    QCOMPARE(info.m_sIP, getConfiguredIp());
    QCOMPARE(info.m_nPort, 6310);
}

void test_modman_config::secServiceConnectionInfo()
{
    ModulemanagerConfigTest::supportOeTests();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    NetworkConnectionInfo info = mmConfig->getSecConnectionInfo();
    QCOMPARE(info.m_sIP, getConfiguredIp());
    QCOMPARE(info.m_nPort, 6305);
}

void test_modman_config::resmanServiceConnectionInfo()
{
    ModulemanagerConfigTest::supportOeTests();

    ModulemanagerConfig* mmConfig = ModulemanagerConfig::getInstance();
    NetworkConnectionInfo info = mmConfig->getResmanConnectionInfo();
    QCOMPARE(info.m_sIP, getConfiguredIp());
    QCOMPARE(info.m_nPort, 6312);
}

QString test_modman_config::getConfiguredIp()
{
    return QString(ZC_SERVICES_IP).isEmpty() ? "127.0.0.1" : ZC_SERVICES_IP;
}
