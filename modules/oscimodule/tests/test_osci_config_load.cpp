#include "test_osci_config_load.h"
#include "oscimoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_osci_config_load)

void test_osci_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_OSCIMODULE) + "/" + "com5003-oscimodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_osci_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_OSCIMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        OSCIMODULE::cOsciModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
