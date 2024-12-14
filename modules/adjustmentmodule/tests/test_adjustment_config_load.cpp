#include "test_adjustment_config_load.h"
#include "adjustmentmoduleconfiguration.h"
#include <testloghelpers.h>
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_adjustment_config_load)

void test_adjustment_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_ADJUSTMENTMODULE) + "/" + "com5003-adjustmentmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_adjustment_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_ADJUSTMENTMODULE) + "/" + "com5003-adjustmentmodule.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<cAdjustmentModuleConfiguration> conf = std::make_unique<cAdjustmentModuleConfiguration>();
    conf->setConfiguration(tmpXmlConfigFile.readAll());

    QCOMPARE(conf->isConfigured(), true);
}

void test_adjustment_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_ADJUSTMENTMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        cAdjustmentModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        cAdjustmentModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
