#include "test_sec1_config_load.h"
#include "sec1moduleconfiguration.h"
#include <testloghelpers.h>
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sec1_config_load)

void test_sec1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SEC1MODULE) + "/" + "com5003-sec1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sec1_config_load::allFilesLoaded()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SEC1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SEC1MODULE::cSec1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}


// from here on basic loading / validation worka so we can load once only
struct PowConfig
{
    QString filename;
    std::shared_ptr<SEC1MODULE::cSec1ModuleConfiguration> config;
};

class ConfFileLoader
{
public:
    static QVector<PowConfig> &get() {
        if(m_confFiles.isEmpty()) {
            QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SEC1MODULE)).entryInfoList(QStringList() << "*.xml");
            m_confFiles.resize(fileList.count());
            for(int i=0; i<fileList.count(); i++) {
                QString filename = fileList[i].fileName();
                m_confFiles[i].filename = filename;
                m_confFiles[i].config = std::make_shared<SEC1MODULE::cSec1ModuleConfiguration>();
                QFile configFile(fileList[i].absoluteFilePath());
                configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
                m_confFiles[i].config->setConfiguration(configFile.readAll());
            }
        }
        return m_confFiles;
    }
private:
    static QVector<PowConfig> m_confFiles;
};

QVector<PowConfig> ConfFileLoader::m_confFiles;

void test_sec1_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SEC1MODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        SEC1MODULE::cSec1ModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        SEC1MODULE::cSec1ModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
