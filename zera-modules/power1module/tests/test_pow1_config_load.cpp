#include "test_pow1_config_load.h"
#include "power1moduleconfiguration.h"
#include "power1moduleconfigdata.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_pow1_config_load)

void test_pow1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_pow1_config_load::allFilesLoaded()
{
    // Note: Some checks are done within cPower1ModuleConfiguration and failures fire Q_ASSERT
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER1MODULE::cPower1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

void test_pow1_config_load::modePhaseListCountSameAsArraySize()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        POWER1MODULE::cPower1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QCOMPARE(conf.getConfigurationData()->m_measmodePhaseList.size(), conf.getConfigurationData()->m_measmodePhaseCount);
    }
}

void test_pow1_config_load::modePhaseListPlausis()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
        POWER1MODULE::cPower1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        int measSystemCount = conf.getConfigurationData()->m_sMeasSystemList.count();
        for(auto &entry : conf.getConfigurationData()->m_measmodePhaseList) {
            QVERIFY(!entry.isEmpty());
            QStringList modePhaseList = entry.split(",");
            QCOMPARE(modePhaseList.size(), 2);
            QVERIFY(conf.getConfigurationData()->m_sMeasmodeList.contains(modePhaseList[0]));
            QCOMPARE(modePhaseList[1].size(), measSystemCount);
        }
    }
}
