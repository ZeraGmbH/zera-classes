#include "test_power2_config_load.h"
#include "power2moduleconfigdata.h"
#include "power2moduleconfiguration.h"
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_power2_config_load)

void test_power2_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_PATH) + "/" + "com5003-power2moduleCED.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_power2_config_load::allFilesLoaded()
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


// from here on basic loading / validation worka so we can load once only
struct PowConfig
{
    QString filename;
    std::shared_ptr<POWER2MODULE::cPower2ModuleConfiguration> config;
};

class ConfFileLoader
{
public:
    static QVector<PowConfig> &get() {
        if(m_confFiles.isEmpty()) {
            QFileInfoList fileList = QDir(QStringLiteral(CONFIG_PATH)).entryInfoList(QStringList() << "*.xml");
            m_confFiles.resize(fileList.count());
            for(int i=0; i<fileList.count(); i++) {
                QString filename = fileList[i].fileName();
                m_confFiles[i].filename = filename;
                m_confFiles[i].config = std::make_shared<POWER2MODULE::cPower2ModuleConfiguration>();
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

void test_power2_config_load::modeListCountSameAsArraySize()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));
        QCOMPARE(config.config->getConfigurationData()->m_sMeasmodeList.size(), config.config->getConfigurationData()->m_nMeasModeCount);
    }
}
