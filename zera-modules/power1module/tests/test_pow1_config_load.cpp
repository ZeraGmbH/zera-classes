#include "test_pow1_config_load.h"
#include "power1moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_pow1_config_load)

void test_pow1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_pow1_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power1module.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<POWER1MODULE::cPower1ModuleConfiguration> conf = std::make_unique<POWER1MODULE::cPower1ModuleConfiguration>();

    QSignalSpy spy(conf.get(), &cBaseModuleConfiguration::configXMLDone);
    conf->setConfiguration(tmpXmlConfigFile.readAll());
    QCOMPARE(spy.count(), 1);
}
