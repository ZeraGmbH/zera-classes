#include "testmodulemanager.h"
#include "testallservicescom5003.h"
#include "testallservicesmt310s2.h"
#include "modulemanagerconfigtest.h"
#include "jsonsessionloadertest.h"
#include <testfactoryi2cctrl.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
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
}

void TestModuleManager::startAllTestServices(QString deviceName, bool initialAdjPermission)
{
    if (m_mockAllServices)
        m_mockAllServices = nullptr;

    if(deviceName == "mt310s2" || deviceName == "mt581s2")
        m_mockAllServices = std::make_unique<TestAllServicesMt310s2>(
            deviceName,
            std::make_shared<TestFactoryI2cCtrl>(initialAdjPermission));
    else if(deviceName == "com5003")
        m_mockAllServices = std::make_unique<TestAllServicesCom5003>(
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

QList<ZeraModules::ModuleData *> TestModuleManager::getModuleList()
{
    return m_moduleList;
}

ZeraModules::VirtualModule *TestModuleManager::getModule(QString uniqueName, int entityId)
{
    for(const auto &moduleInfo : qAsConst(m_moduleList)) {
        if(moduleInfo->m_uniqueName == uniqueName && moduleInfo->m_moduleId == entityId)
            return moduleInfo->m_module;
    }
    return nullptr;
}

const QByteArray TestModuleManager::getLastStoredConfig()
{
    return *m_configDataLastSaved;
}

VeinTcp::AbstractTcpNetworkFactoryPtr TestModuleManager::getTcpNetworkFactory()
{
    return m_tcpNetworkFactory;
}

bool TestModuleManager::modulesReady()
{
    return !m_moduleStartLock;
}

QStringList TestModuleManager::getModuleFileNames()
{
    QString strBuildPath = MODULE_PLUGIN_BUILD_PATH;
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
