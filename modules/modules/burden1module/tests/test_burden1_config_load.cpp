#include "test_burden1_config_load.h"
#include "burden1moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_burden1_config_load)

void test_burden1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_BURDEN1MODULE) + "/" + "mt310s2-burden1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_burden1_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_BURDEN1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        BURDEN1MODULE::cBurden1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
