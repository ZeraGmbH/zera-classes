#include "test_dft_module_regression.h"
#include <timemachineobject.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_dft_module_regression)

void test_dft_module_regression::cleanup()
{
    if(m_modMan)
        m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    m_factoryActualValueGen = nullptr;
    TimeMachineObject::feedEventLoop();
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
}

static int constexpr dftEntityId = 1050;

void test_dft_module_regression::minimalSession()
{
    setupServices(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(dftEntityId));
}

void test_dft_module_regression::moduleConfigFromResource()
{
    setupServices(":/session-dft-moduleconfig-from-resource.json");
    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(dftEntityId));
}

void test_dft_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    setupServices(":/session-dft-moduleconfig-from-resource.json");
    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << dftEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int currentPhaseCount = 4;
static constexpr int rmsResultCount = voltagePhaseNeutralCount + voltagePhasePhaseCount + currentPhaseCount;

void test_dft_module_regression::checkActualValueCount()
{
    setupServices(":/session-dft-moduleconfig-from-resource.json");

    TestActValManInTheMiddlePtr actValueGenerator = m_factoryActualValueGen->getActValGeneratorRmsTest(dftEntityId);
    QStringList actValName = actValueGenerator->getValueChannelList();
    QCOMPARE(actValName.count(), rmsResultCount);
}

void test_dft_module_regression::injectActualValues()
{
    setupServices(":/session-dft-moduleconfig-from-resource.json");

    TestActValManInTheMiddlePtr actValueGenerator = m_factoryActualValueGen->getActValGeneratorRmsTest(dftEntityId);
    QVector<float> actValues(rmsResultCount*2);
    for(int i = 0; i<rmsResultCount*2; i++)
        actValues[i] = i;
    actValueGenerator->onNewActualValues(&actValues);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << dftEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

void test_dft_module_regression::setupServices(QString sessionFileName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_factoryActualValueGen = std::make_shared<TestFactoryActValManInTheMiddle>();
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_factoryActualValueGen, true);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllServiceMocks("mt310s2");
    m_modMan->loadSession(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}
