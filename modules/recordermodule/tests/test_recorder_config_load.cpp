#include "test_recorder_config_load.h"
#include "recordermoduleconfigdata.h"
#include "recordermoduleconfiguration.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_recorder_config_load)

void test_recorder_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RECORDERMODULE)).entryInfoList(QStringList() << "*.xml");
    QVERIFY(!fileList.isEmpty());
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        RecorderModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
        RecorderModuleConfigData* configData = conf.getConfigurationData();
        QCOMPARE(configData->m_entityCount, 2);
    }
}

void test_recorder_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RECORDERMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        RecorderModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        RecorderModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
