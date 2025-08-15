#include "test_dosage_config_load.h"
#include "dosagemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_dosage_config_load)

void test_dosage_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_DOSAGEMODULE) + "/" + "mt310s2-dosagemodule-DEF.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_dosage_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_DOSAGEMODULE)).entryInfoList(QStringList() << "*.xml");
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

void test_dosage_config_load::allFilesFormatCheckLight()
{
    // This test only validate roughly the format of the text contents. That is why it is called light.
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_DOSAGEMODULE)).entryInfoList(QStringList() << "*.xml");
    for (const auto &fileInfo : fileList)
    {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        DOSAGEMODULE::cDosageModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());

        DOSAGEMODULE::cDosageModuleConfigData *modulConfig;
        modulConfig = conf.getConfigurationData();

        for(int i = 0; i < modulConfig->m_nDosageSystemCount; i++) {
            QVERIFY((modulConfig->m_DosageSystemConfigList[i].m_nEntity > 0) && (modulConfig->m_DosageSystemConfigList[i].m_nEntity <= 9999));
            QVERIFY(modulConfig->m_DosageSystemConfigList[i].m_ComponentName.contains("ACT_"));
        }

    }

}
