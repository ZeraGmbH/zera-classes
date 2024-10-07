#include "test_thdn_config_load.h"
#include "thdnmoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_thdn_config_load)

void test_thdn_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_THDNMODULE) + "/" + "com5003-thdnmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_thdn_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_THDNMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        THDNMODULE::cThdnModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
