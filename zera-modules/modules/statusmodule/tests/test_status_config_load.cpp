#include "test_status_config_load.h"
#include "statusmoduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_status_config_load)

void test_status_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "com5003-statusmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_status_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "com5003-statusmodule.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<STATUSMODULE::cStatusModuleConfiguration> conf = std::make_unique<STATUSMODULE::cStatusModuleConfiguration>();

    QSignalSpy spy(conf.get(), &cBaseModuleConfiguration::configXMLDone);
    conf->setConfiguration(tmpXmlConfigFile.readAll());
    QCOMPARE(spy.count(), 1);
}
