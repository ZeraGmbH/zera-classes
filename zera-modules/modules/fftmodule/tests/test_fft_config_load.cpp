#include "test_fft_config_load.h"
#include "fftmoduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_fft_config_load)

void test_fft_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_FFTMODULE) + "/" + "com5003-fftmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_fft_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_FFTMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        FFTMODULE::cFftModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
