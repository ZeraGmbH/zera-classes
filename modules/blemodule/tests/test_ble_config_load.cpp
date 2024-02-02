#include "test_ble_config_load.h"
#include "blemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_ble_config_load)

void test_ble_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_BLEMODULE) + "/" + "blemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_ble_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_BLEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        BLEMODULE::cBleModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
