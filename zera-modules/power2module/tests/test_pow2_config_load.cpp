#include "test_pow2_config_load.h"
#include "power2moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_pow2_config_load)

void test_pow2_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power2moduleCED.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_pow2_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power2moduleCED.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<POWER2MODULE::cPower2ModuleConfiguration> conf = std::make_unique<POWER2MODULE::cPower2ModuleConfiguration>();

    QSignalSpy spy(conf.get(), &cBaseModuleConfiguration::configXMLDone);
    conf->setConfiguration(tmpXmlConfigFile.readAll());
    QCOMPARE(spy.count(), 1);
}
