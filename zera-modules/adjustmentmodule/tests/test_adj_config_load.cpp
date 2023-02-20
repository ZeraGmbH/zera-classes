#include "test_adj_config_load.h"
#include "adjustmentmoduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adj_config_load);

void test_adj_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-adjustmentmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_adj_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-adjustmentmodule.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<cAdjustmentModuleConfiguration> conf = std::make_unique<cAdjustmentModuleConfiguration>();

    QSignalSpy spy(conf.get(), &cBaseModuleConfiguration::configXMLDone);
    conf->setConfiguration(tmpXmlConfigFile.readAll());
    QCOMPARE(spy.count(), 1);
}
