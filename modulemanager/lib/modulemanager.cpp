#include "modulemanager.h"
#include "modulemanagerconfig.h"
#include "moduledata.h"
#include "licensesystem.h"
#include "demoallservicescom5003.h"
#include "demoallservicesmt310s2.h"

#include <ve_eventsystem.h>
#include <vsc_scriptsystem.h>

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
bool ModuleManager::m_runningInTest = false;


ModuleManager::ModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo, QObject *parent) :
    QObject(parent),
    m_setupFacade(setupFacade),
    m_demo(demo),
    m_moduleStartLock(false)
{
    m_timerAllModulesLoaded.start();

    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    QStringList sessionList = mmConfig->getAvailableSessions();

    QStringList entryList = QDir(m_sessionPath).entryList(QStringList({"*.json"}));
    QSet<QString> fileSet(entryList.begin(), entryList.end());
    QSet<QString> expectedSet(sessionList.begin(), sessionList.end());
    if(fileSet.contains(expectedSet))
    {
        m_sessionsAvailable = sessionList;
    }
    else
    {
        QSet<QString> missingSessions = expectedSet;
        missingSessions.subtract(fileSet);
        qCritical() << "Missing session file(s)" << missingSessions;
    }
    qDebug() << "sessions available:" << m_sessionsAvailable;

    if(m_demo && !QString(ZC_SERVICES_IP).isEmpty()) {
        qWarning() << "Running in demo mode, so ZC_SERIVCES_IP must be empty! It is set to:" << ZC_SERVICES_IP;
        Q_ASSERT(false);
    }
}

ModuleManager::~ModuleManager()
{
    foreach(ModuleData *toDelete, m_moduleList)
    {
        m_factoryTable.value(toDelete->m_uniqueName)->destroyModule(toDelete->m_reference);
        delete toDelete;
    }
    m_moduleList.clear();
}

QStringList ModuleManager::getModuleFileNames()
{
    QString modulePath = MODMAN_MODULE_PATH;
    qDebug("Loading modules from %s", qPrintable(modulePath));
    QDir moduleDir(modulePath);
    QStringList fullNames;
    for(auto &name : moduleDir.entryList(QDir::Files))
        fullNames.append(moduleDir.absoluteFilePath(name));
    return fullNames;
}

bool ModuleManager::loadAllAvailableModulePlugins()
{
    bool retVal = false;
    for(auto& fileName : getModuleFileNames()) {
        QPluginLoader loader(fileName);
        MeasurementModuleFactory *module = qobject_cast<MeasurementModuleFactory *>(loader.instance());
        qDebug() << "Analyzing:" << loader.fileName() << "\nfile is a library?" << QLibrary::isLibrary(fileName) << "loaded:" << loader.isLoaded();
        if (module) {
            module->setModuleNumerator(std::make_unique<ModuleGroupNumerator>());
            retVal=true;
            m_factoryTable.insert(module->getFactoryName(), module);
        }
        else
            qWarning() << "Error string:\n" << loader.errorString();
    }
    qInfo("Modules analysed after %llims", m_timerAllModulesLoaded.elapsed());
    return retVal;
}

void ModuleManager::loadScripts(VeinScript::ScriptSystem *t_scriptSystem)
{
    //load builtin qml-scripts
    const QDir virtualFiles(":/qml");
    const QStringList scriptList = virtualFiles.entryList();
    for(const QString &scriptFilePath : scriptList)
    {
        const QString dataLocation = QString("%1/%2").arg(virtualFiles.path(), scriptFilePath);
        qDebug() << "Loading script:" << dataLocation;
        if(t_scriptSystem->loadScriptFromFile(dataLocation) == false)
        {
            qWarning() << "Error loading script file:" << scriptFilePath;
        }
    }
}

bool ModuleManager::modulesReady()
{
    return !m_moduleStartLock;
}

void ModuleManager::setupConnections()
{
    //start the next module as soon as the PAR_SerialNr component is avaiable
    connect(m_setupFacade->getLicenseSystem(), &LicenseSystem::sigSerialNumberInitialized, this, &ModuleManager::delayedModuleStartNext);

    QObject::connect(&m_sessionLoader, &JsonSessionLoader::sigLoadModule, this, &ZeraModules::ModuleManager::startModule);
    QObject::connect(this, &ZeraModules::ModuleManager::sigSessionSwitched, &m_sessionLoader, &JsonSessionLoader::loadSession);

    m_setupFacade->getModuleManagerController()->initOnce();
    QObject::connect(this, &ZeraModules::ModuleManager::sigModulesLoaded, m_setupFacade->getModuleManagerController(), &ModuleManagerController::initializeEntity);
    QObject::connect(m_setupFacade->getModuleManagerController(), &ModuleManagerController::sigChangeSession, this, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(m_setupFacade->getModuleManagerController(), &ModuleManagerController::sigModulesPausedChanged, this, &ZeraModules::ModuleManager::setModulesPaused);
}

void ModuleManager::loadDefaultSession()
{
    ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
    changeSessionFile(mmConfig->getDefaultSession());
}

void ModuleManager::startModule(const QString & uniqueModuleName, const QString & t_xmlConfigPath, const QByteArray &t_xmlConfigData, int moduleEntityId, int moduleNum)
{
    // do not allow starting until all modules are shut down
    if(m_moduleStartLock == false) {
        qInfo("Starting module %s...", qPrintable(uniqueModuleName));
        MeasurementModuleFactory *tmpFactory = m_factoryTable.value(uniqueModuleName);
        if(tmpFactory && m_setupFacade->getLicenseSystem()->isSystemLicensed(uniqueModuleName)) {
            qDebug() << "Creating module:" << uniqueModuleName << "with id:" << moduleEntityId << "with config file:" << t_xmlConfigPath;
            VirtualModule *tmpModule = tmpFactory->createModule(moduleEntityId, m_setupFacade->getStorageSystem(), m_demo, moduleNum);
            if(tmpModule) {
                connect(tmpModule, &VirtualModule::addEventSystem, this, &ModuleManager::onModuleEventSystemAdded);
                tmpModule->setConfiguration(t_xmlConfigData);
                connect(tmpModule, SIGNAL(moduleDeactivated()), this, SLOT(onStartModuleDelete()));
                connect(tmpModule, &VirtualModule::moduleActivated, this, [this](){
                    m_moduleStartLock=false;
                    delayedModuleStartNext();
                });
                connect(tmpModule, &VirtualModule::moduleError, this, &ModuleManager::onModuleError);

                m_moduleStartLock = true;
                tmpModule->startModule();
                ModuleData *moduleData = new ModuleData(tmpModule, uniqueModuleName, t_xmlConfigPath, QByteArray(), moduleEntityId, moduleNum);
                connect(tmpModule, &VirtualModule::parameterChanged, this, [this, moduleData](){
                    saveModuleConfig(moduleData);
                });
                m_moduleList.append(moduleData);
            }
        }
        else if(m_setupFacade->getLicenseSystem()->serialNumberIsInitialized()) {
            if(tmpFactory != nullptr)
                qWarning() << "Skipping module:" << uniqueModuleName << "No license found!";
            else
                qWarning() << "Could not create module:" << uniqueModuleName << "!";
            onModuleStartNext();
        }
        else {//wait for serial number initialization
            qInfo("No serialno - enqueue module %s...", qPrintable(uniqueModuleName));
            m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueModuleName, t_xmlConfigPath, t_xmlConfigData, moduleEntityId, moduleNum));
        }
    }
    else {
        qInfo("Locked - enqueue module %s...", qPrintable(uniqueModuleName));
        m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueModuleName, t_xmlConfigPath, t_xmlConfigData, moduleEntityId, moduleNum));
    }
}

void ModuleManager::destroyModules()
{
    m_setupFacade->clearSystems();
    if(!m_moduleList.isEmpty()) {
        m_moduleStartLock = true;
        QElapsedTimer destroyTimer;
        destroyTimer.start();
        for(int i = m_moduleList.length()-1; i>=0; i--) { // seems we don't want to destroy _SYSTEM...
            VirtualModule *toDestroy = m_moduleList.at(i)->m_reference;
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

bool ZeraModules::ModuleManager::loadSession(const QString sessionFileNameFull)
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
    for(ModuleData *module : qAsConst(m_moduleList))
    {
        if(t_paused)
        {
            qDebug() << "pausing module:" << module->m_uniqueName;
            module->m_reference->stopModule();
        }
        else
        {
            qDebug() << "unpausing module:" << module->m_uniqueName;
            module->m_reference->startModule();
        }
    }
}

void ModuleManager::onStartModuleDelete()
{
    VirtualModule *toDelete = qobject_cast<VirtualModule*>(QObject::sender());
    if(toDelete) {
        ModuleData *tmpData = ModuleData::findByReference(m_moduleList, toDelete);
        if(tmpData) {
            qInfo() << "Start delete module:" << tmpData->m_uniqueName;
            connect(toDelete, &VirtualModule::destroyed, this, &ModuleManager::checkModuleList);
            toDelete->deleteLater();
        }
        else
            qWarning() << "Could not find data for VirtualModule" << toDelete;
    }
}

void ModuleManager::checkModuleList(QObject *object)
{
    VirtualModule *toDelete = static_cast<VirtualModule*>(object);
    if(toDelete) {
        ModuleData *tmpData = ModuleData::findByReference(m_moduleList, toDelete);
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
    {
        onModuleStartNext();
    }
}

void ModuleManager::onModuleStartNext()
{
    m_moduleStartLock = false;
    if(m_deferredStartList.length() > 0) {
        ModuleData *tmpData = m_deferredStartList.dequeue();
        startModule(tmpData->m_uniqueName, tmpData->m_configPath, tmpData->m_configData, tmpData->m_moduleId, tmpData->m_moduleNum);
        delete tmpData;
    }
    else {
        if(!m_runningInTest) {
            ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
            mmConfig->setDefaultSession(m_sessionFile);
        }
        qInfo("All modules started within %llims", m_timerAllModulesLoaded.elapsed());
        emit sigModulesLoaded(m_sessionFile, m_sessionsAvailable);
    }
}

void ModuleManager::onModuleError(const QString &t_error)
{
    qWarning() << "Module error:" << t_error;
}

void ModuleManager::onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem)
{
    m_setupFacade->addSystem(t_eventSystem);
}

void ModuleManager::setDemoServices(QString deviceName)
{
    if (m_mockAllServices)
        m_mockAllServices = nullptr;

    if(deviceName == "mt310s2")
        m_mockAllServices = std::make_unique<DemoAllServicesMt310s2>();
    else if(deviceName == "com5003")
        m_mockAllServices = std::make_unique<DemoAllServicesCom5003>();
}


void ModuleManager::saveModuleConfig(ModuleData *t_moduleData)
{
    if(m_runningInTest)
        return;
    QByteArray configData = t_moduleData->m_reference->getConfiguration();

    if(configData.isEmpty() == false)
    {
        QSaveFile f; //if the application is closed while writing it will not end up with empty files due to truncate

        f.setFileName(t_moduleData->m_configPath);
        //qDebug() << "Storing configuration for module:" << t_moduleData->m_uniqueName << "in file:" << f.fileName();
        f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered);
        if(f.isOpen() && f.isWritable())
        {
            f.write(configData);
            f.commit(); //writes into the permanent file and closes the file descriptors
        }
        else
        {
            qWarning() << "Failed to write configuration file:" << f.fileName() << "error:" << f.errorString();
        }
    }
    else
    {
        qWarning() << "Configuration could not be retrieved from module:" << t_moduleData->m_uniqueName;
    }
}

}
