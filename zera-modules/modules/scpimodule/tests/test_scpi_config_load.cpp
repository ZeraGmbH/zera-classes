#include "test_scpi_config_load.h"
#include "scpimoduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_scpi_config_load)

void test_scpi_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCE_PATH) + "/" + "com5003-scpimodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_scpi_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCE_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SCPIMODULE::cSCPIModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
