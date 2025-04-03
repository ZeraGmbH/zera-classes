#include "test_sfc_config_load.h"
#include "sfcmoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_sfc_config_load)

void test_sfc_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SFCMODULE) + "/" + "sfcmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sfc_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SFCMODULE)).entryInfoList(QStringList() << "*.xml");
    for (const auto &fileInfo : fileList)
    {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SFCMODULE::cSfcModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
