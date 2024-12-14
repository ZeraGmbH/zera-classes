#include "test_power1_config_load.h"
#include "power1moduleconfiguration.h"
#include "power1dspmodefunctioncatalog.h"
#include "measmode.h"
#include "measmodebroker.h"
#include "meassytemchannels.h"
#include "power1moduleconfigdata.h"
#include <testloghelpers.h>
#include <memory>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_power1_config_load)

void test_power1_config_load::fileFound()
{
    QFile tmpXmlConfigFile(QStringLiteral(CONFIG_SOURCES_POWER1MODULE) + "/" + "com5003-power1module.xml");
    QVERIFY(tmpXmlConfigFile.exists());
}

void test_power1_config_load::allFilesLoaded()
{
    // Note: Some checks are done within cPower1ModuleConfiguration and failures fire Q_ASSERT
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER1MODULE)).entryInfoList(QStringList() << "*.xml");
    for(const auto &fileInfo : fileList) {
        QFile configFile(fileInfo.absoluteFilePath());
        qInfo("Load %s...", qPrintable(configFile.fileName()));
        QVERIFY(configFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
        POWER1MODULE::cPower1ModuleConfiguration conf;
        conf.setConfiguration(configFile.readAll());
        QVERIFY(conf.isConfigured());
    }
}


// from here on basic loading / validation worka so we can load once only
struct PowConfig
{
    QString filename;
    std::shared_ptr<POWER1MODULE::cPower1ModuleConfiguration> config;
};

class ConfFileLoader
{
public:
    static QVector<PowConfig> &get() {
        if(m_confFiles.isEmpty()) {
            QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER1MODULE)).entryInfoList(QStringList() << "*.xml");
            m_confFiles.resize(fileList.count());
            for(int i=0; i<fileList.count(); i++) {
                QString filename = fileList[i].fileName();
                m_confFiles[i].filename = filename;
                m_confFiles[i].config = std::make_shared<POWER1MODULE::cPower1ModuleConfiguration>();
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

void test_power1_config_load::modeListCountSameAsArraySize()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));
        QCOMPARE(config.config->getConfigurationData()->m_sMeasmodeList.size(), config.config->getConfigurationData()->m_nMeasModeCount);
    }
}

void test_power1_config_load::modePhaseListCountSameAsArraySize()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));
        QCOMPARE(config.config->getConfigurationData()->m_measmodePhaseList.size(), config.config->getConfigurationData()->m_measmodePhaseCount);
    }
}

void test_power1_config_load::modePhaseListPlausis()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));
        int measSystemCount = config.config->getConfigurationData()->m_sMeasSystemList.count();
        for(auto &entry : config.config->getConfigurationData()->m_measmodePhaseList) {
            QVERIFY(!entry.isEmpty());
            QStringList modePhaseList = entry.split(",");
            QCOMPARE(modePhaseList.size(), 2);
            QVERIFY(config.config->getConfigurationData()->m_sMeasmodeList.contains(modePhaseList[0]));
            QCOMPARE(modePhaseList[1].size(), measSystemCount);
        }
    }
}

void test_power1_config_load::measSystemsValid()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));
        for(const auto &systemStr : qAsConst(config.config->getConfigurationData()->m_sMeasSystemList)) {
            QStringList voltageCurrent = systemStr.split(",");
            QCOMPARE(voltageCurrent.size(), 2);
            QStringList allowedEntries;
            for(int i=0; i<2*4; i++)
                allowedEntries.append(QString("m%1").arg(i));
            QVERIFY(allowedEntries.contains(voltageCurrent[0]));
            QVERIFY(allowedEntries.contains(voltageCurrent[1]));
        }
    }
}

void test_power1_config_load::allModesProperlyLoadedFromConfig()
{
    for(const auto &config : qAsConst(ConfFileLoader::get())) {
        qInfo("Check %s...", qPrintable(config.filename));

        // A stripped copy of cPower1ModuleMeasProgram::setDspCmdList()
        MeasSystemChannels measChannelPairList;
        POWER1MODULE::cPower1ModuleConfigData *confdata = config.config->getConfigurationData();
        for(int i=0; i<confdata->m_sMeasSystemList.count(); i++) {
            MeasSystemChannel measChannel;
            measChannel.voltageChannel = measChannelPairList.size()*2;
            measChannel.currentChannel = measChannelPairList.size()*2+1;
            measChannelPairList.append(measChannel);
        }

        DspChainIdGen dspChainGen;
        int measSytemCount = confdata->m_sMeasSystemList.count();
        MeasModeBroker measBroker(Power1DspModeFunctionCatalog::get(measSytemCount), dspChainGen);

        for (int i = 0; i < confdata->m_nMeasModeCount; i++) {
            cMeasModeInfo mInfo = MeasModeCatalog::getInfo(confdata->m_sMeasmodeList.at(i));
            QVERIFY2(mInfo.isValid(), qPrintable(confdata->m_sMeasmodeList.at(i)));

            MeasModeBroker::BrokerReturn brokerReturn = measBroker.getMeasMode(mInfo.getName(), measChannelPairList);
            QVERIFY2(brokerReturn.isValid(), qPrintable(confdata->m_sMeasmodeList.at(i)));

            std::shared_ptr<MeasMode> mode = std::make_shared<MeasMode>(mInfo.getName(),
                                                                        brokerReturn.dspSelectCode,
                                                                        measSytemCount,
                                                                        std::move(brokerReturn.phaseStrategy));
            QVERIFY2(mode->isValid(), qPrintable(confdata->m_sMeasmodeList.at(i)));
            QCOMPARE(mode->getMeasSysCount(), confdata->m_sMeasSystemList.count());
        }
    }
}

void test_power1_config_load::writtenXmlIsStillValid()
{
    const QFileInfoList fileList = QDir(QStringLiteral(CONFIG_SOURCES_POWER1MODULE)).entryInfoList(QStringList() << "*.xml");
    bool allOk = true;
    for(const auto &fileInfo : fileList) {
        QByteArray xmlConfOrig = TestLogHelpers::loadFile(fileInfo.absoluteFilePath());
        POWER1MODULE::cPower1ModuleConfiguration moduleConfig;
        moduleConfig.setConfiguration(xmlConfOrig);
        if(!moduleConfig.isConfigured()) {
            allOk = false;
            qWarning("Load config failed for %s", qPrintable(fileInfo.fileName()));
        }
        QByteArray configDataExport = moduleConfig.exportConfiguration();
        POWER1MODULE::cPower1ModuleConfiguration moduleConfig2;
        moduleConfig2.setConfiguration(configDataExport);
        if(!moduleConfig2.isConfigured()) {
            allOk = false;
            qWarning("Reload config failed for %s", qPrintable(fileInfo.fileName()));
        }
    }
    QVERIFY(allOk);
}
