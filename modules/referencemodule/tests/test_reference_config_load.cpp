#include "test_reference_config_load.h"
#include "referencemoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_reference_config_load)

void test_reference_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_REFERENCEMODULE) + "/" + "com5003-referencemodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_reference_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_REFERENCEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        REFERENCEMODULE::cReferenceModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
