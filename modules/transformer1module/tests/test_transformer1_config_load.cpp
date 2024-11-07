#include "test_transformer1_config_load.h"
#include "transformer1moduleconfiguration.h"
#include <QTest>

QTEST_MAIN(test_transformer1_config_load)

void test_transformer1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_TRANSFORMER1MODULE) + "/" + "mt310s2-transformer1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_transformer1_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_TRANSFORMER1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        TRANSFORMER1MODULE::cTransformer1ModuleConfiguration conf;
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

void test_transformer1_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_TRANSFORMER1MODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = loadFile(fileInfo.absoluteFilePath());
        TRANSFORMER1MODULE::cTransformer1ModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        TRANSFORMER1MODULE::cTransformer1ModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
