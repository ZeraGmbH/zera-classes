#include "test_dft_config_load.h"
#include "dftmoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_dft_config_load)

void test_dft_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_DFTMODULE) + "/" + "com5003-dftmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_dft_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_DFTMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        DFTMODULE::cDftModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
