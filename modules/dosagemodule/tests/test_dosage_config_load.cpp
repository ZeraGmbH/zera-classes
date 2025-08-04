#include "test_dosage_config_load.h"
#include "dosagemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_dosage_config_load)

void test_dosage_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_DOSAGEMODULE) + "/" + "dosagemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_dosage_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_DOSAGEMODULE)).entryInfoList(QStringList() << "*.xml");
    for (const auto &fileInfo : fileList)
    {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        DOSAGEMODULE::cDosageModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
