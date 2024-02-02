#include "test_sem1_config_load.h"
#include "sem1moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sem1_config_load)

void test_sem1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_SEM1MODULE) + "/" + "com5003-sem1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_sem1_config_load::allFilesLoaded()
{
    QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SEM1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        SEM1MODULE::cSem1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}


// from here on basic loading / validation worka so we can load once only
struct PowConfig
{
    QString filename;
    std::shared_ptr<SEM1MODULE::cSem1ModuleConfiguration> config;
};

class ConfFileLoader
{
public:
    static QVector<PowConfig> &get() {
        if(m_confFiles.isEmpty()) {
            QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_SEM1MODULE)).entryInfoList(QStringList() << "*.xml");
            m_confFiles.resize(fileList.count());
            for(int i=0; i<fileList.count(); i++) {
                QString filename = fileList[i].fileName();
                m_confFiles[i].filename = filename;
                m_confFiles[i].config = std::make_shared<SEM1MODULE::cSem1ModuleConfiguration>();
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

