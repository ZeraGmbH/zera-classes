#include "test_sample_config_load.h"
#include "samplemoduleconfiguration.h"
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_sample_config_load)

void test_sample_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE) + "/" + "com5003-samplemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sample_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SAMPLEMODULE::cSampleModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}

void test_sample_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        SAMPLEMODULE::cSampleModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        SAMPLEMODULE::cSampleModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
