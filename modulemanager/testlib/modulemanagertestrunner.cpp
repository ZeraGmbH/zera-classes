#include "modulemanagertestrunner.h"
#include "testdspvalues.h"
#include "vf_client_component_setter.h"
#include <mocklxdmconfigfilegenerator.h>
#include <contentsetsotherfromcontentsetsconfig.h>
#include <contentsetszeraallfrommodmansessions.h>
#include <loggercontentsetconfig.h>
#include <testsqlitedb.h>
#include <timemachineobject.h>
#include <QDir>

ModuleManagerTestRunner::ModuleManagerTestRunner(const QString &sessionFileName,
                                                 bool initialAdjPermission,
                                                 const QString &deviceName,
                                                 const LxdmSessionChangeParam &lxdmParam,
                                                 bool addVfLogger,
                                                 bool cleanupLxdmFiles) :
    m_licenseSystem(std::make_unique<TestLicenseSystem>()),
    m_modmanFacade(std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(), false, lxdmParam)),
    m_serviceInterfaceFactory(std::make_shared<TestFactoryServiceInterfaces>()),
    m_modMan(std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory)),
    m_vfCmdEventHandlerSystem(std::make_shared<VfCmdEventHandlerSystem>()),
    m_cleanupLxdmFiles(cleanupLxdmFiles)
{
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllTestServices(deviceName, initialAdjPermission);
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());
    if(addVfLogger)
        setupVfLogger();
    if(!sessionFileName.isEmpty())
        start(sessionFileName);
}

ModuleManagerTestRunner::~ModuleManagerTestRunner()
{
    if(m_modMan)
        m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    TimeMachineObject::feedEventLoop();
    m_serviceInterfaceFactory = nullptr;
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
    if(m_cleanupLxdmFiles)
        MockLxdmConfigFileGenerator::cleanup();
}

void ModuleManagerTestRunner::setupVfLogger()
{
    m_testSignaller = std::make_unique<TestDbAddSignaller>();
    const VeinLogger::DBFactory sqliteFactory = [this]() {
        return std::make_shared<TestSQLiteDB>(m_testSignaller.get());
    };

    m_dataLoggerSystem = std::make_unique<VeinLogger::DatabaseLogger>(
        m_modmanFacade->getStorageSystem(),
        sqliteFactory,
        this,
        QList<int>()
            << 0    /* SYSTEM */
            << 1150 /* STATUS */);

    QString OE_MODMAN_CONTENTSET_PATH = QDir::cleanPath(QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_CONTENTSET_PATH));
    QString OE_MODMAN_SESSION_PATH = QDir::cleanPath(QString(OE_INSTALL_ROOT) + "/" + QString(MODMAN_SESSION_PATH));
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(OE_MODMAN_CONTENTSET_PATH, std::make_shared<ContentSetsOtherFromContentSetsConfig>());
    VeinLogger::LoggerContentSetConfig::setJsonEnvironment(OE_MODMAN_SESSION_PATH, std::make_shared<ContentSetsZeraAllFromModmanSessions>());

    m_dataLoggerSystemInitialized = false;
    connect(m_modmanFacade->getLicenseSystem(), &LicenseSystemInterface::sigSerialNumberInitialized, this, [&](){
        if(m_licenseSystem->isSystemLicensed(m_dataLoggerSystem->entityName()))
        {
            if(!m_dataLoggerSystemInitialized)
            {
                m_dataLoggerSystemInitialized = true;
                qInfo("Starting DataLoggerSystem...");
                m_modmanFacade->addSubsystem(m_dataLoggerSystem.get());
            }
        }
    });

    m_modmanFacade->getLicenseSystem()->setDeviceSerial("foo");
    TimeMachineObject::feedEventLoop();
}

void ModuleManagerTestRunner::start(const QString &sessionFileName)
{
    m_currentSessionFileName = sessionFileName;
    m_modMan->changeSessionFile(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}

QString ModuleManagerTestRunner::getSessionFileName() const
{
    return m_currentSessionFileName;
}

QList<TestModuleManager::TModuleInstances> ModuleManagerTestRunner::getInstanceCountsOnModulesDestroyed()
{
    return m_modMan->getInstanceCountsOnModulesDestroyed();
}

int ModuleManagerTestRunner::getModuleConfigWriteCounts() const
{
    return m_modMan->getModuleConfigWriteCounts();
}

void ModuleManagerTestRunner::setRangeGetSetDelay(int rangeGetSetDelay)
{
    m_modMan->setRangeGetSetDelay(rangeGetSetDelay);
}

ZDspServer *ModuleManagerTestRunner::getDspServer()
{
    return m_modMan->getDspServer();
}

cSEC1000dServer *ModuleManagerTestRunner::getSecServer()
{
    return m_modMan->getSecServer();
}

void ModuleManagerTestRunner::fireHotplugInterrupt(const AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap &infoMap)
{
    m_modMan->fireHotplugInterrupt(infoMap);
}

AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap ModuleManagerTestRunner::getCurrentHotplugMap() const
{
    return m_modMan->getCurrentHotplugMap();
}

cSenseSettingsPtr ModuleManagerTestRunner::getSenseSettings()
{
    return m_modMan->getSenseSettings();
}

void ModuleManagerTestRunner::addStandardEmobControllers(const QStringList &channelAliases)
{
    m_modMan->addStandardEmobControllers(channelAliases);
}

void ModuleManagerTestRunner::removeAllHotplugDevices()
{
    m_modMan->removeAllHotplugDevices();
    TimeMachineObject::feedEventLoop();
}

void ModuleManagerTestRunner::addClamps(const QList<AbstractMockAllServices::clampParam> &clampParams)
{
    m_modMan->addClamps(clampParams);
    TimeMachineObject::feedEventLoop();
}

VeinStorage::AbstractEventSystem *ModuleManagerTestRunner::getVeinStorageSystem()
{
    return m_modmanFacade->getStorageSystem();
}

VeinStorage::AbstractDatabase *ModuleManagerTestRunner::getVeinStorageDb()
{
    return m_modmanFacade->getStorageSystem()->getDb();
}

TestDspInterfacePtr ModuleManagerTestRunner::findDspInterfaceByType(DspInterfaceInjectableTypes injectType)
{
    return m_serviceInterfaceFactory->findDspInterfaceByType(injectType);
}

TestDspInterfacePtr ModuleManagerTestRunner::findDspInterfaceByEntityId(int entityId)
{
    return m_serviceInterfaceFactory->findDspInterfaceByEntityId(entityId);
}

QList<TestDspInterfacePtr> ModuleManagerTestRunner::findAllDspInterfaceByType(DspInterfaceInjectableTypes injectType)
{
    return m_serviceInterfaceFactory->findAllDspInterfaceByType(injectType);
}

QMap<int, QList<TestDspInterfacePtr> > ModuleManagerTestRunner::getAllDspInterfaces()
{
    return m_serviceInterfaceFactory->getAllDspInterfaces();
}

void ModuleManagerTestRunner::fireActualValues()
{
    constexpr double testvoltage = 120;
    constexpr double testcurrent = 10;
    constexpr double testangle = 0;
    constexpr double testfrequency = 50;

    const QString session = getSessionFileName();
    bool hasDcDft = session.contains("ref-session");  // ATTOW it is just com5003-ref-session
    int dftOrder = hasDcDft ? 0 : 1;

    TestDspValues dspValues(findDspInterfaceByType(INJECT_DFT)->getValueList(), dftOrder);
    if(session.contains("dc") || hasDcDft)
        dspValues.setAllValuesSymmetricDc(testvoltage, testcurrent);
    else
        dspValues.setAllValuesSymmetricAc(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireAllActualValues(
        findDspInterfaceByType(INJECT_DFT),
        findDspInterfaceByType(INJECT_FFT),
        findDspInterfaceByType(INJECT_RANGE_PROGRAM),
        findDspInterfaceByType(INJECT_RMS),
        findAllDspInterfaceByType(INJECT_POWER1));
}

VfCmdEventHandlerSystemPtr ModuleManagerTestRunner::getVfCmdEventHandlerSystemPtr()
{
    return m_vfCmdEventHandlerSystem;
}

ModuleManagerSetupFacade *ModuleManagerTestRunner::getModManFacade()
{
    return m_modmanFacade.get();
}

ZeraModules::VirtualModule *ModuleManagerTestRunner::getModule(int entityId)
{
    return m_modMan->getModule(entityId);
}

void ModuleManagerTestRunner::setVfComponent(int entityId, const QString &componentName, const QVariant &newValue)
{
    QVariant oldValue = getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_modmanFacade->getStorageSystem()->sigSendEvent(event); // could be any event system
    TimeMachineObject::feedEventLoop();
}

QVariant ModuleManagerTestRunner::getVfComponent(int entityId, const QString &componentName)
{
    return getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
}
