#include "test_power3_config_load.h"
#include "power3moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_power3_config_load)

void test_power3_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_POWER3MODULE) + "/" + "mt310s2-power3module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_power3_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER3MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER3MODULE::cPower3ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}


static QByteArray loadFile(QString fileName)
{
    QByteArray fileData;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly)) {
        fileData = file.readAll();
        file.close();
    }
    return fileData;
}

void test_power3_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER3MODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = loadFile(fileInfo.absoluteFilePath());
        POWER3MODULE::cPower3ModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        POWER3MODULE::cPower3ModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
