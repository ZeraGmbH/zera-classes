#include "test_transformer1_config_load.h"
#include "transformer1moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_transformer1_config_load)

void test_transformer1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_TRANSFORMER1MODULE) + "/" + "mt310s2-transformer1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_transformer1_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_TRANSFORMER1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        TRANSFORMER1MODULE::cTransformer1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
