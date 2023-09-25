#include "test_power3_config_load.h"
#include "power3moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_power3_config_load)

void test_power3_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_POWER3MODULE) + "/" + "mt310s2-power3module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_power3_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER3MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER3MODULE::cPower3ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
