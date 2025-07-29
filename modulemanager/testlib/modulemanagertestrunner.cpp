#include "modulemanagertestrunner.h"
#include "vf_client_component_setter.h"
#include <contentsetsotherfromcontentsetsconfig.h>
#include <contentsetszeraallfrommodmansessions.h>
#include <loggercontentsetconfig.h>
#include <testsqlitedb.h>
#include <timemachineobject.h>
#include <QDir>

ModuleManagerTestRunner::ModuleManagerTestRunner(QString sessionFileName, bool initialAdjPermission, QString deviceName, LxdmSessionChangeParam lxdmParam)
{
    m_licenseSystem = std::make_unique<TestLicenseSystem>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get(),
                                                                false,
                                                                lxdmParam);
    m_serviceInterfaceFactory = std::make_shared<TestFactoryServiceInterfaces>();
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllTestServices(deviceName, initialAdjPermission);
    m_vfCmdEventHandlerSystem = std::make_shared<VfCmdEventHandlerSystem>();
    m_modmanFacade->addSubsystem(m_vfCmdEventHandlerSystem.get());
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

void ModuleManagerTestRunner::start(QString sessionFileName)
{
    m_modMan->changeSessionFile(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}

QList<TestModuleManager::TModuleInstances> ModuleManagerTestRunner::getInstanceCountsOnModulesDestroyed()
{
    return m_modMan->getInstanceCountsOnModulesDestroyed();
}

VeinStorage::AbstractEventSystem *ModuleManagerTestRunner::getVeinStorageSystem()
{
    return m_modmanFacade->getStorageSystem();
}

TestDspInterfacePtr ModuleManagerTestRunner::getDspInterface(int entityId,
                                                             TestFactoryServiceInterfaces::DSPInterfaceType dspInterfaceType)
{
    return m_serviceInterfaceFactory->getInterface(entityId, dspInterfaceType);
}

const QList<TestDspInterfacePtr> &ModuleManagerTestRunner::getDspInterfaceList() const
{
    return m_serviceInterfaceFactory->getInterfaceList();
}

VfCmdEventHandlerSystemPtr ModuleManagerTestRunner::getVfCmdEventHandlerSystemPtr()
{
    return m_vfCmdEventHandlerSystem;
}

ModuleManagerSetupFacade *ModuleManagerTestRunner::getModManFacade()
{
    return m_modmanFacade.get();
}

ZeraModules::VirtualModule *ModuleManagerTestRunner::getModule(QString uniqueName, int entityId)
{
    return m_modMan->getModule(uniqueName, entityId);
}

void ModuleManagerTestRunner::setVfComponent(int entityId, QString componentName, QVariant newValue)
{
    QVariant oldValue = getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_modmanFacade->getStorageSystem()->sigSendEvent(event); // could be any event system
    TimeMachineObject::feedEventLoop();
}

QVariant ModuleManagerTestRunner::getVfComponent(int entityId, QString componentName)
{
    return getVeinStorageSystem()->getDb()->getStoredValue(entityId, componentName);
}
