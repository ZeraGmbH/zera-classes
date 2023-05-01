#include "test_rms_config_load.h"
#include "rmsmoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_rms_config_load)

void test_rms_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_RMSMODULE) + "/" + "com5003-rmsmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_rms_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RMSMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        RMSMODULE::cRmsModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
