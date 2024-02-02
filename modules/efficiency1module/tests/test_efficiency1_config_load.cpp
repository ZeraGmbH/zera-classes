#include "test_efficiency1_config_load.h"
#include "efficiency1moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_efficiency1_config_load)

void test_efficiency1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_EFFICIENCY1MODULE) + "/" + "mt310s2-efficiency1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_efficiency1_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_EFFICIENCY1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        EFFICIENCY1MODULE::cEfficiency1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
