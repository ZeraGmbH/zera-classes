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
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_REFERENCEMODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        REFERENCEMODULE::cReferenceModuleConfiguration conf;
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

void test_reference_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_REFERENCEMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = loadFile(fileInfo.absoluteFilePath());
        REFERENCEMODULE::cReferenceModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        REFERENCEMODULE::cReferenceModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
