#include "test_range_config_load.h"
#include "rangemoduleconfigdata.h"
#include "rangemoduleconfiguration.h"
#include <testloghelpers.h>
#include <QTest>
#include <memory.h>

QTEST_MAIN(test_range_config_load)

void test_range_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "com5003-rangemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_range_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RANGEMODULE)).entryInfoList(QStringList() << "*.xml");
    QVERIFY(!fileList.isEmpty());
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        RANGEMODULE::cRangeModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

void test_range_config_load::invertPhaseStateLoaded()
{
    QFile configFile(QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/mt310s2-rangemodule.xml");
    RANGEMODULE::cRangeModuleConfiguration conf;
    QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    conf.setConfiguration(configFile.readAll());
    QVERIFY(conf.isConfigured());
    RANGEMODULE::cRangeModuleConfigData* confData = conf.getConfigurationData();
    QCOMPARE(confData->m_adjustConfPar.m_senseChannelInvertParameter.size(), confData->m_nChannelCount);
    for(auto item : confData->m_adjustConfPar.m_senseChannelInvertParameter) {
        QVERIFY(!item.m_nActive);
    }
}

void test_range_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RANGEMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        RANGEMODULE::cRangeModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }

        QByteArray configDataExport = moduleConfig.exportConfiguration();
        RANGEMODULE::cRangeModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
