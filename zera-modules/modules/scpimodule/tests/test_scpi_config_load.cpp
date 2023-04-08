#include "test_scpi_config_load.h"
#include "scpimoduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_scpi_config_load)

void test_scpi_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-scpimodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_scpi_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-scpimodule.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<SCPIMODULE::cSCPIModuleConfiguration> conf = std::make_unique<SCPIMODULE::cSCPIModuleConfiguration>();

    QSignalSpy spy(conf.get(), &cBaseModuleConfiguration::configXMLDone);
    conf->setConfiguration(tmpXmlConfigFile.readAll());
    QCOMPARE(spy.count(), 1);
}
