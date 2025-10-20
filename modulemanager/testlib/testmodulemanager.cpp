#include "testmodulemanager.h"
#include "testallservicescom5003.h"
#include "testallservicesmt310s2.h"
#include "test_plugin_paths.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include "moduleactivist.h"
#include <interface_p.h>
#include <vfmodulemetadata.h>
#include <vfmodulecomponent.h>
#include <testfactoryi2cctrl.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <scpinode.h>
#include <QDir>

bool TestModuleManager::prepareOe()
{
    ModulemanagerConfigTest::supportOeTests();
    JsonSessionLoaderTest::supportOeTests();
    pointToInstalledSessionFiles();
    return true;
}

void TestModuleManager::enableTests()
{
    QDir dir(SOURCE_STATE_PATH);
    dir.removeRecursively();
    TimerFactoryQtForTest::enableTest();
    prepareOe();
}

void TestModuleManager::pointToInstalledSessionFiles()
{
    m_sessionPath = QDir::cleanPath(
                    QString(OE_INSTALL_ROOT) + "/" +
                    QString(MODMAN_SESSION_PATH));
}

TestModuleManager::TestModuleManager(ModuleManagerSetupFacade *setupFacade,
                                     AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                     VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                     std::shared_ptr<QByteArray> configDataLastSaved) :
    ModuleManager(
        setupFacade,
        serviceInterfaceFactory,
        tcpNetworkFactory,
        // This is a hack to modify static test environment before ModuleManager starts using them
        prepareOe()),
    m_configDataLastSaved(configDataLastSaved)
{
    enableTests();
    connect(this, &ModuleManager::sigModulesUnloaded, this,
            &TestModuleManager::onAllModulesDestroyed);
}

void TestModuleManager::startAllTestServices(QString deviceName, bool initialAdjPermission)
{
    if (m_testAllServices)
        m_testAllServices = nullptr;

    if(deviceName == "mt310s2" || deviceName == "mt581s2")
        m_testAllServices = std::make_unique<TestAllServicesMt310s2>(
            deviceName,
            std::make_shared<TestFactoryI2cCtrl>(initialAdjPermission));
    else if(deviceName == "com5003")
        m_testAllServices = std::make_unique<TestAllServicesCom5003>(
            std::make_shared<TestFactoryI2cCtrl>(initialAdjPermission));
}

void TestModuleManager::destroyModulesAndWaitUntilAllShutdown()
{
    destroyModules();
    waitUntilModulesAreReady();
}

void TestModuleManager::waitUntilModulesAreReady()
{
    do
        TimeMachineObject::feedEventLoop();
    while(!modulesReady());
}

ZeraModules::VirtualModule *TestModuleManager::getModule(QString uniqueName, int entityId)
{
    for(const auto &moduleInfo : qAsConst(*m_moduleDataList)) {
        if(moduleInfo->m_uniqueName == uniqueName && moduleInfo->m_moduleId == entityId)
            return moduleInfo->m_module;
    }
    return nullptr;
}

const QByteArray TestModuleManager::getLastStoredConfig()
{
    return *m_configDataLastSaved;
}

ZDspServer *TestModuleManager::getDspServer()
{
    return m_testAllServices->getZdsp1dServer();
}

void TestModuleManager::setRangeGetSetDelay(int rangeGetSetDelay)
{
    m_testAllServices->setRangeGetSetDelay(rangeGetSetDelay);
}

void TestModuleManager::onAllModulesDestroyed()
{
    m_instantCountsOnModulesDestroyed.append( {
        VfModuleMetaData::getInstanceCount(),
        VfModuleComponent::getInstanceCount(),
        VeinEvent::EventSystem::getInstanceCount(),
        cModuleActivist::getInstanceCount(),
        Zera::cInterfacePrivate::getInstanceCount(),
        cDspVar::getInstanceCount(),
        ScpiNode::getInstanceCount(),
        ScpiObject::getInstanceCount(),
        VeinTcp::TcpPeer::getInstanceCount()
    } );
}

QList<TestModuleManager::TModuleInstances> TestModuleManager::getInstanceCountsOnModulesDestroyed()
{
    return m_instantCountsOnModulesDestroyed;
}

bool TestModuleManager::modulesReady()
{
    return !m_moduleStartLock;
}

QStringList TestModuleManager::getModuleFileNames()
{
    QString strBuildPath = TestPluginPaths::getPaths();
    QStringList buildPaths = strBuildPath.split(" ", Qt::SkipEmptyParts);
    QStringList plugins;
    for(auto &buildPath : buildPaths) {
        QStringList libFiles = QDir(buildPath, "*.so").entryList();
        QString libFullPath = QDir::cleanPath(buildPath + "/" + libFiles[0]);
        plugins.append(libFullPath);
    }
    return plugins;
}

void TestModuleManager::saveModuleConfig(ZeraModules::ModuleData *moduleData)
{
    *m_configDataLastSaved = moduleData->m_module->getConfiguration();
}
