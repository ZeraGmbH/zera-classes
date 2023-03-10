#include "test_pow2_config_load.h"
#include "power2moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_pow2_config_load)

void test_pow2_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power2moduleCED.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_pow2_config_load::allFilesLoaded()
{
    // Note: Some checks are done within cPower2ModuleConfiguration and failures fire Q_ASSERT
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER2MODULE::cPower2ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}
