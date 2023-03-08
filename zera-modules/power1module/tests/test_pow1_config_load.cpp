#include "test_pow1_config_load.h"
#include "power1moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_pow1_config_load)

void test_pow1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_pow1_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER1MODULE::cPower1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
