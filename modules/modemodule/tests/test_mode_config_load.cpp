#include "test_mode_config_load.h"
#include "modemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_mode_config_load)

void test_mode_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_MODEMODULE) + "/" + "mt310s2-modemoduleAC.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_mode_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_MODEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        MODEMODULE::cModeModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
