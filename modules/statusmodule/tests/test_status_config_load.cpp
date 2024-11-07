#include "test_status_config_load.h"
#include "statusmoduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_status_config_load)

void test_status_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "com5003-statusmodule.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_status_config_load::fileLoaded()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "com5003-statusmodule.xml");
    QVERIFY(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    std::unique_ptr<STATUSMODULE::cStatusModuleConfiguration> conf = std::make_unique<STATUSMODULE::cStatusModuleConfiguration>();
    conf->setConfiguration(tmpXmlConfigFile.readAll());

    QCOMPARE(conf->isConfigured(), true);
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

void test_status_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_STATUSMODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = loadFile(fileInfo.absoluteFilePath());
        STATUSMODULE::cStatusModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        STATUSMODULE::cStatusModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
