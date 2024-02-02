#include "test_range_config_load.h"
#include "rangemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_range_config_load)

void test_range_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "com5003-rangemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_range_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_RANGEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        RANGEMODULE::cRangeModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
