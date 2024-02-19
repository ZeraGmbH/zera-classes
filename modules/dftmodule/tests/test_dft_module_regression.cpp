#include "test_dft_module_regression.h"
#include "dftmodulemeasprogram.h"
#include "factoryserviceinterfacessingleton.h"
#include "testfactoryserviceinterfaces.h"
#include <timemachineobject.h>
#include <vf_core_stack_client.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_dft_module_regression)

void test_dft_module_regression::initTestCase()
{
    FactoryServiceInterfacesSingleton::setInstance(std::make_unique<TestFactoryServiceInterfaces>());
}

void test_dft_module_regression::cleanup()
{
    TestFactoryServiceInterfaces* factory = static_cast<TestFactoryServiceInterfaces*>(FactoryServiceInterfacesSingleton::getInstance());
    factory->clearInterfaceList();
    if(m_modMan)
        m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
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
    setupServices(":/session-dft-no-movingwindow-no-ref.json");
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

    setupServices(":/session-dft-no-movingwindow-no-ref.json");
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
static constexpr int dftResultCount = voltagePhaseNeutralCount + voltagePhasePhaseCount + currentPhaseCount;

void test_dft_module_regression::checkActualValueCount()
{
    setupServices(":/session-dft-no-movingwindow-no-ref.json");

    TestFactoryServiceInterfaces* factory = static_cast<TestFactoryServiceInterfaces*>(FactoryServiceInterfacesSingleton::getInstance());
    const QList<TestDspInterfacePtr>& dspInterfaces = factory->getInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QStringList valueList = dspInterfaces[0]->getValueList();
    QCOMPARE(valueList.count(), dftResultCount);
}

void test_dft_module_regression::injectActualValuesNoReferenceChannel()
{
    setupServices(":/session-dft-no-movingwindow-no-ref.json");

    TestFactoryServiceInterfaces* factory = static_cast<TestFactoryServiceInterfaces*>(FactoryServiceInterfacesSingleton::getInstance());
    const QList<TestDspInterfacePtr>& dspInterfaces = factory->getInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i;

    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual-no-ref.json");
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

void test_dft_module_regression::injectActualValuesReferenceChannelUL1()
{
    setupServices(":/session-dft-no-movingwindow-ref.json");

    TestFactoryServiceInterfaces* factory = static_cast<TestFactoryServiceInterfaces*>(FactoryServiceInterfacesSingleton::getInstance());
    const QList<TestDspInterfacePtr>& dspInterfaces = factory->getInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i+1;

    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual-refUL1.json");
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

void test_dft_module_regression::injectActualValuesReferenceChannelUL2()
{
    setupServices(":/session-dft-no-movingwindow-ref.json");
    setReferenceChannel("UL2");

    TestFactoryServiceInterfaces* factory = static_cast<TestFactoryServiceInterfaces*>(FactoryServiceInterfacesSingleton::getInstance());
    const QList<TestDspInterfacePtr>& dspInterfaces = factory->getInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i+1;

    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual-refUL2.json");
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

void test_dft_module_regression::setReferenceChannel(QString channel)
{
    m_vfCmdEventHandlerSystem = std::make_shared<VfCmdEventHandlerSystem>();
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());

    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(dftEntityId);
    m_vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_RefChannel", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent("UL1", channel);
    TimeMachineObject::feedEventLoop();
}

void test_dft_module_regression::setupServices(QString sessionFileName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), true);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllServiceMocks("mt310s2");
    m_modMan->loadSession(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}
