#include "test_periodaverage_config_load.h"
#include "periodaveragemoduleconfiguration.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_periodaverage_config_load)

void test_periodaverage_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_PERIODAVERAGEMODULE) + "/" + "mt310s2-periodaveragemodule-emob-dc.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_periodaverage_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_PERIODAVERAGEMODULE)).entryInfoList(QStringList() << "*.xml");
    QVERIFY(!fileList.isEmpty());
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        PERIODAVERAGEMODULE::PeriodAverageModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

void test_periodaverage_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_PERIODAVERAGEMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        PERIODAVERAGEMODULE::PeriodAverageModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        PERIODAVERAGEMODULE::PeriodAverageModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}

void test_periodaverage_config_load::measPeriodHigherThanMaxPeriod()
{
    QFile configFile(":/configs/all-channels-maxperiod-5-periods-out-of-limit.xml");
    QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    PERIODAVERAGEMODULE::PeriodAverageModuleConfiguration conf;
    conf.setConfiguration(configFile.readAll());
    QVERIFY(!conf.isConfigured());
}
