#include "test_api_config_load.h"
#include "apimoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_api_config_load)

void test_api_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_APIMODULE) + "/" + "apimodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_api_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_APIMODULE)).entryInfoList(QStringList() << "*.xml");
    for (const auto &fileInfo : fileList)
    {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        APIMODULE::cApiModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
