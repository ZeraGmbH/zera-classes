#include "test_sample_config_load.h"
#include "samplemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_sample_config_load)

void test_sample_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE) + "/" + "com5003-samplemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sample_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SAMPLEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SAMPLEMODULE::cSampleModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
