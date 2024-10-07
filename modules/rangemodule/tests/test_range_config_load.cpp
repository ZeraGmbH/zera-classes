#include "test_range_config_load.h"
#include "rangemoduleconfigdata.h"
#include "rangemoduleconfiguration.h"
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
