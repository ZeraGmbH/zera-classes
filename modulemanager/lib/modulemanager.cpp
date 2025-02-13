#include "modulemanager.h"
#include "modulemanagerconfig.h"
#include "moduledata.h"
#include "licensesystem.h"
#include "demoallservicescom5003.h"
#include "demoallservicesmt310s2.h"

#include <ve_eventsystem.h>

#include <QPluginLoader>
#include <abstractmodulefactory.h>
#include <QDir>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDebug>

namespace ZeraModules {

QString ModuleManager::m_sessionPath = MODMAN_SESSION_PATH;

QString ModuleManager::getInstalledSessionPath()
{
    return m_sessionPath;
}

ModuleManager::ModuleManager(ModuleManagerSetupFacade *setupFacade,
                             AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                             bool moduleDemoMode, QObject *parent) :
    QObject(parent),
    m_moduleStartLock(false),
    m_setupFacade(setupFacade),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_moduleDemoMode(moduleDemoMode)
{
    m_timerAllModulesLoaded.start();

    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    QStringList sessionList = mmConfig->getAvailableSessions();

    QStringList entryList = QDir(m_sessionPath).entryList(QStringList({"*.json"}));
    QSet<QString> fileSet(entryList.begin(), entryList.end());
    QSet<QString> expectedSet(sessionList.begin(), sessionList.end());
    if(fileSet.contains(expectedSet))
        m_sessionsAvailable = sessionList;
    else {
        QSet<QString> missingSessions = expectedSet;
        missingSessions.subtract(fileSet);
        qCritical() << "Missing session file(s)" << missingSessions;
    }
    if(m_moduleDemoMode && !QString(ZC_SERVICES_IP).isEmpty())
        qFatal("Running modules in demo mode, so ZC_SERIVCES_IP must be empty! It is set to: " ZC_SERVICES_IP);
}

ModuleManager::~ModuleManager()
{
    for(ModuleData *module : qAsConst(m_moduleList)) {
        m_factoryTable.value(module->m_uniqueName)->destroyModule(module->m_module);
        delete module;
    }
    m_moduleList.clear();
}

QStringList ModuleManager::getModuleFileNames()
{
    QString modulePath = MODMAN_MODULE_PATH;
    QDir moduleDir(modulePath);
    QStringList fullNames;
    for(auto &name : moduleDir.entryList(QDir::Files))
        fullNames.append(moduleDir.absoluteFilePath(name));
    return fullNames;
}

void ZeraModules::ModuleManager::handleFinalModuleLoaded()
{
    saveDefaultSession();
    m_moduleSharedObjects = nullptr; // all modules are supplied
    qInfo("All modules started within %llims", m_timerAllModulesLoaded.elapsed());
    emit sigModulesLoaded(m_sessionFile, m_sessionsAvailable);
}

bool ModuleManager::loadAllAvailableModulePlugins()
{
    bool retVal = false;
    for(auto& fileName : getModuleFileNames()) {
        QPluginLoader loader(fileName);
        AbstractModuleFactory *module = qobject_cast<AbstractModuleFactory *>(loader.instance());
        if (module) {
            module->setModuleNumerator(std::make_unique<ModuleGroupNumerator>());
            retVal=true;
            m_factoryTable.insert(module->getFactoryName(), module);
        }
        else
            qWarning() << "Error string:\n" << loader.errorString();
    }
    qInfo("Modules analyse took %llims", m_timerAllModulesLoaded.elapsed());
    return retVal;
}

void ModuleManager::setupConnections()
{
    //start the next module as soon as the PAR_SerialNr component is avaiable
    connect(m_setupFacade->getLicenseSystem(), &LicenseSystem::sigSerialNumberInitialized, this, &ModuleManager::delayedModuleStartNext);

    QObject::connect(&m_sessionLoader, &JsonSessionLoader::sigLoadModule, this, &ZeraModules::ModuleManager::startModule);
    QObject::connect(this, &ZeraModules::ModuleManager::sigSessionSwitched, &m_sessionLoader, &JsonSessionLoader::loadSession);

    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    QString configFileName = mmConfig->getConfigFileNameFull();
    m_setupFacade->getSystemModuleEventSystem()->setConfigFileName(configFileName);
    m_setupFacade->getSystemModuleEventSystem()->setAvailableSessionList(m_sessionsAvailable);

    m_setupFacade->getSystemModuleEventSystem()->initOnce();
    QObject::connect(this, &ZeraModules::ModuleManager::sigModulesLoaded, m_setupFacade->getSystemModuleEventSystem(), &SystemModuleEventSystem::initializeEntity);
    QObject::connect(m_setupFacade->getSystemModuleEventSystem(), &SystemModuleEventSystem::sigChangeSession, this, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(m_setupFacade->getSystemModuleEventSystem(), &SystemModuleEventSystem::sigModulesPausedChanged, this, &ZeraModules::ModuleManager::setModulesPaused);
}

void ModuleManager::loadDefaultSession()
{
    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    changeSessionFile(mmConfig->getDefaultSession());
}

void ModuleManager::createCommonModuleParam()
{
    if(!m_moduleSharedObjects) {
        ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
        ModuleNetworkParamsPtr networkParams = std::make_shared<ModuleNetworkParams>(
            m_tcpNetworkFactory,
            mmConfig->getPcbConnectionInfo(),
            mmConfig->getDspConnectionInfo(),
            mmConfig->getSecConnectionInfo(),
            mmConfig->getResmanConnectionInfo());
        ChannelRangeObserver::SystemObserverPtr channelRangeObserver =
            std::make_shared<ChannelRangeObserver::SystemObserver>(mmConfig->getPcbConnectionInfo(), m_tcpNetworkFactory);
        m_moduleSharedObjects = std::make_shared<ModuleSharedData>(networkParams,
                                                                   m_serviceInterfaceFactory,
                                                                   channelRangeObserver,
                                                                   m_moduleDemoMode);
    }
}

VirtualModule *ZeraModules::ModuleManager::createModule(const QString &xmlConfigPath,
                                                        int moduleEntityId,
                                                        int moduleNum,
                                                        const QString &uniqueName,
                                                        AbstractModuleFactory *tmpFactory)
{
    QByteArray xmlConfigData;
    if(!xmlConfigPath.isEmpty()) {
        QFile tmpXmlConfigFile(xmlConfigPath);
        if(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            xmlConfigData = tmpXmlConfigFile.readAll();
            tmpXmlConfigFile.close();
        }
        else {
            qCritical() << "Error opening config file for module:" << uniqueName << "path:" << xmlConfigPath;
            return nullptr;
        }
    }
    ModuleFactoryParam moduleParam(moduleEntityId,
                                   moduleNum,
                                   xmlConfigData,
                                   m_setupFacade->getStorageSystem(),
                                   m_moduleSharedObjects);
    VirtualModule *tmpModule = tmpFactory->createModule(moduleParam);
    if(tmpModule) {
        connect(tmpModule, &VirtualModule::addEventSystem, this, &ModuleManager::onModuleEventSystemAdded);
        connect(tmpModule, &VirtualModule::moduleDeactivated, this, &ModuleManager::onStartModuleDelete);
        connect(tmpModule, &VirtualModule::moduleActivated, this, [this](){
            m_moduleStartLock = false;
            delayedModuleStartNext();
        });
    }
    return tmpModule;
}

void ZeraModules::ModuleManager::doStartModule(VirtualModule *tmpModule,
                                               const QString &uniqueName,
                                               const QString &xmlConfigPath,
                                               int moduleEntityId,
                                               int moduleNum)
{
    ModuleData *moduleData = new ModuleData(tmpModule,
                                            uniqueName,
                                            xmlConfigPath,
                                            moduleEntityId,
                                            moduleNum);
    connect(tmpModule, &VirtualModule::parameterChanged, this, [this, moduleData](){
        saveModuleConfig(moduleData);
    });
    m_moduleList.append(moduleData);
    tmpModule->startModule();
}

void ModuleManager::startModule(const QString &uniqueName,
                                const QString &xmlConfigPath,
                                int moduleEntityId,
                                int moduleNum)
{
    // do not allow starting until all modules are shut down
    if(m_moduleStartLock == false) {
        AbstractModuleFactory *tmpFactory = m_factoryTable.value(uniqueName);
        if(tmpFactory && m_setupFacade->getLicenseSystem()->isSystemLicensed(uniqueName)) {
            const QFileInfo confFileInfo(xmlConfigPath);
            qInfo("Creating module: %s / EntityId: %i / Config: %s",
                  qPrintable(uniqueName),
                  moduleEntityId,
                  qPrintable(confFileInfo.fileName()));
            createCommonModuleParam();
            Q_ASSERT(!m_currModulePrepareTask);
            m_currModulePrepareTask = tmpFactory->getModulePrepareTask(m_moduleSharedObjects);
            connect(m_currModulePrepareTask.get(), &TaskTemplate::sigFinish, [=](bool ok) {
                VirtualModule *tmpModule = nullptr;
                if(ok)
                    tmpModule = createModule(xmlConfigPath,
                                             moduleEntityId,
                                             moduleNum,
                                             uniqueName,
                                             tmpFactory);
                if(tmpModule)
                    doStartModule(tmpModule, uniqueName, xmlConfigPath, moduleEntityId, moduleNum);
                else
                    qCritical("Error creating module: %s / %i", qPrintable(uniqueName), moduleEntityId);
                m_currModulePrepareTask = nullptr;
            });
            m_moduleStartLock = true;
            m_currModulePrepareTask->start();
        }
        else if(m_setupFacade->getLicenseSystem()->serialNumberIsInitialized()) {
            if(tmpFactory != nullptr)
                qWarning() << "Skipping module:" << uniqueName << "No license found!";
            else
                qWarning() << "Could not create module:" << uniqueName << "!";
            onModuleStartNext();
        }
        else {//wait for serial number initialization
            qInfo("No serialno - enqueue module %s...", qPrintable(uniqueName));
            m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueName, xmlConfigPath, moduleEntityId, moduleNum));
        }
    }
    else {
        // All but first are enqueued - misconception? - spam just for thos e working on..
        //qInfo("Locked - enqueue module %s...", qPrintable(uniqueModuleName));
        m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueName, xmlConfigPath, moduleEntityId, moduleNum));
    }
}

void ModuleManager::destroyModules()
{
    m_setupFacade->clearModuleSystems();
    if(!m_moduleList.isEmpty()) {
        m_moduleStartLock = true;
        QElapsedTimer destroyTimer;
        destroyTimer.start();
        for(int i = m_moduleList.length()-1; i>=0; i--) { // seems we don't want to destroy _SYSTEM...
            VirtualModule *toDestroy = m_moduleList.at(i)->m_module;
            QString tmpModuleName = m_moduleList.at(i)->m_uniqueName;
            if(m_factoryTable.contains(tmpModuleName)) {
                qInfo() << "Destroying module:" << tmpModuleName;
                m_factoryTable.value(tmpModuleName)->destroyModule(toDestroy);
            }
        }
        qInfo("All modules destroyed within %llims", destroyTimer.elapsed());
        m_timerAllModulesLoaded.start();
    }
}

bool ModuleManager::loadSession(const QString sessionFileNameFull)
{
    bool sessionSwitched = false;
    if(QFile::exists(sessionFileNameFull)) {
        if(m_moduleStartLock == false) { // do not mess up the state machines
            sessionSwitched = true;
            destroyModules();
            emit sigSessionSwitched(sessionFileNameFull);
        }
        else
            qWarning() << "Cannot switch sessions while session change already is in progress";
    }
    else
        qWarning() << "Session file not found:" << sessionFileNameFull << "Search path:" << m_sessionPath;
    return sessionSwitched;
}

void ModuleManager::changeSessionFile(const QString &newSessionFile)
{
    if(m_sessionFile != newSessionFile) {
        const QString sessionFileNameFull = QDir::cleanPath(QString("%1/%2").arg(m_sessionPath, newSessionFile));
        if(loadSession(sessionFileNameFull))
            m_sessionFile = newSessionFile;
        else
            qFatal("loadSession failed!!!");
    }
}

void ModuleManager::setModulesPaused(bool t_paused)
{
    for(ModuleData *module : qAsConst(m_moduleList)) {
        if(t_paused)
            module->m_module->stopModule();
        else
            module->m_module->startModule();
    }
}

void ModuleManager::onStartModuleDelete()
{
    VirtualModule *toDelete = qobject_cast<VirtualModule*>(QObject::sender());
    if(toDelete) {
        ModuleData *tmpData = ModuleData::findModuleByPointer(m_moduleList, toDelete);
        if(tmpData) {
            qInfo() << "Start delete module:" << tmpData->m_uniqueName;
            connect(toDelete, &VirtualModule::destroyed, this, &ModuleManager::onDestroyModule);
            toDelete->deleteLater();
        }
        else
            qWarning() << "Could not find data for VirtualModule" << toDelete;
    }
}

void ModuleManager::onDestroyModule(QObject *object)
{
    VirtualModule *toDelete = static_cast<VirtualModule*>(object);
    if(toDelete) {
        ModuleData *tmpData = ModuleData::findModuleByPointer(m_moduleList, toDelete);
        if(tmpData) {
            qInfo() << "Deleted module:" << tmpData->m_uniqueName;
            m_moduleList.removeAll(tmpData);
            delete tmpData;
        }
    }
    if(m_moduleList.isEmpty())
        onModuleStartNext();
}

void ModuleManager::delayedModuleStartNext()
{
    if(m_setupFacade->getLicenseSystem()->serialNumberIsInitialized() == true && m_moduleStartLock == false)
        onModuleStartNext();
}

void ZeraModules::ModuleManager::saveDefaultSession()
{
    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    mmConfig->setDefaultSession(m_sessionFile);
}

void ModuleManager::onModuleStartNext()
{
    m_moduleStartLock = false;
    if(m_deferredStartList.length() > 0) {
        ModuleData *tmpData = m_deferredStartList.dequeue();
        startModule(tmpData->m_uniqueName, tmpData->m_configPath, tmpData->m_moduleId, tmpData->m_moduleNum);
        delete tmpData;
    }
    else
        handleFinalModuleLoaded();
}

void ModuleManager::onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem)
{
    m_setupFacade->addModuleSystem(t_eventSystem);
}

void ModuleManager::startAllDemoServices(QString deviceName)
{
    if (m_mockAllServices)
        m_mockAllServices = nullptr;
    if(deviceName == "mt310s2")
        m_mockAllServices = std::make_unique<DemoAllServicesMt310s2>();
    else if(deviceName == "com5003")
        m_mockAllServices = std::make_unique<DemoAllServicesCom5003>();
}


void ModuleManager::saveModuleConfig(ModuleData *moduleData)
{
    QByteArray configData = moduleData->m_module->getConfiguration();
    if(configData.isEmpty() == false) {
        QSaveFile f; //if the application is closed while writing it will not end up with empty files due to truncate
        f.setFileName(moduleData->m_configPath);
        f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered);
        if(f.isOpen() && f.isWritable()) {
            f.write(configData);
            f.commit(); //writes into the permanent file and closes the file descriptors
        }
        else
            qWarning() << "Failed to write configuration file:" << f.fileName() << "error:" << f.errorString();
    }
    else
        qWarning() << "Configuration could not be retrieved from module:" << moduleData->m_uniqueName;
}

}
