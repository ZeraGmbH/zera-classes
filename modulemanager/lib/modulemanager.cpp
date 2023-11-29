#include "modulemanager.h"
#include "modulemanagerconfig.h"
#include "licensesystem.h"

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

namespace ZeraModules
{
class ModuleData {
public:
    ModuleData(VirtualModule *t_ref, const QString &t_name, const QString &t_confPath, const QByteArray &t_confData, int moduleEntityId) :
        m_reference(t_ref),
        m_uniqueName(t_name),
        m_configPath(t_confPath),
        m_configData(t_confData),
        m_moduleId(moduleEntityId)
    {}

    ~ModuleData() {}

    static ModuleData *findByReference(QList<ModuleData*> t_list, VirtualModule *t_ref)
    {
        ModuleData *retVal = nullptr;
        foreach(ModuleData *tmpData, t_list)
        {
            if(tmpData->m_reference == t_ref)
            {
                retVal = tmpData;
                break;
            }
        }
        return retVal;
    }

    VirtualModule *m_reference;
    const QString m_uniqueName;
    const QString m_configPath;
    QByteArray m_configData;
    int m_moduleId;
};

QString ModuleManager::m_sessionPath = MODMAN_SESSION_PATH;
bool ModuleManager::m_runningInTest = false;


ModuleManager::ModuleManager(const QStringList &sessionList, ModuleManagerSetupFacade *setupFacade, QObject *parent) :
    QObject(parent),
    m_moduleStartLock(false),
    m_setupFacade(setupFacade)
{
    m_timerAllModulesLoaded.start();
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

void ModuleManager::setStorage(VeinEvent::StorageSystem *t_storage)
{
    m_storage = t_storage;
}

void ModuleManager::setLicenseSystem(LicenseSystemInterface *t_licenseSystem)
{
    ///@todo move to constructor as the ModuleManager depends on the LicenseSystem
    Q_ASSERT(t_licenseSystem != nullptr);
    m_licenseSystem = t_licenseSystem;
    //start the next module as soon as the PAR_SerialNr component is avaiable
    connect(m_licenseSystem, &LicenseSystem::sigSerialNumberInitialized, this, &ModuleManager::delayedModuleStartNext);
}

void ModuleManager::setDemo(bool demo)
{
    m_demo = demo;
    if(m_demo) {
        ModulemanagerConfig *mmConfig = ModulemanagerConfig::getInstance();
        setMockServices(mmConfig->getDeviceName());
    }
}

bool ModuleManager::areAllModulesShutdown()
{
    return !m_moduleStartLock;
}

void ModuleManager::setModuleManagerControllerConnections()
{
    QObject::connect(this, &ZeraModules::ModuleManager::sigModulesLoaded, m_setupFacade->getModuleManagerController(), &ModuleManagerController::initializeEntity);
    QObject::connect(m_setupFacade->getModuleManagerController(), &ModuleManagerController::sigChangeSession, this, &ZeraModules::ModuleManager::changeSessionFile);
    QObject::connect(m_setupFacade->getModuleManagerController(), &ModuleManagerController::sigModulesPausedChanged, this, &ZeraModules::ModuleManager::setModulesPaused);
}

void ModuleManager::startModule(const QString & uniqueModuleName, const QString & t_xmlConfigPath, const QByteArray &t_xmlConfigData, int moduleEntityId)
{
    // do not allow starting until all modules are shut down
    if(m_moduleStartLock == false)
    {
        qInfo("Starting module %s...", qPrintable(uniqueModuleName));
        MeasurementModuleFactory *tmpFactory=nullptr;

        tmpFactory=m_factoryTable.value(uniqueModuleName);
        if(tmpFactory && m_licenseSystem->isSystemLicensed(uniqueModuleName))
        {
            qDebug() << "Creating module:" << uniqueModuleName << "with id:" << moduleEntityId << "with config file:" << t_xmlConfigPath;
            VirtualModule *tmpModule = tmpFactory->createModule(moduleEntityId, m_storage, m_demo, this);
            if(tmpModule)
            {
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
                ModuleData *moduleData = new ModuleData(tmpModule, uniqueModuleName, t_xmlConfigPath, QByteArray(), moduleEntityId);
                connect(tmpModule, &VirtualModule::parameterChanged, this, [this, moduleData](){
                    saveModuleConfig(moduleData);
                });
                m_moduleList.append(moduleData);
            }
        }
        else if(m_licenseSystem->serialNumberIsInitialized())
        {
            if(tmpFactory != nullptr) {
                qWarning() << "Skipping module:" << uniqueModuleName << "No license found!";
            }
            else {
                qWarning() << "Could not create module:" << uniqueModuleName << "!";
            }
            onModuleStartNext();
        }
        else //wait for serial number initialization
        {
            qInfo("No serialno - enqueue module %s...", qPrintable(uniqueModuleName));
            m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueModuleName, t_xmlConfigPath, t_xmlConfigData, moduleEntityId));
        }
    }
    else
    {
        qInfo("Locked - enqueue module %s...", qPrintable(uniqueModuleName));
        m_deferredStartList.enqueue(new ModuleData(nullptr, uniqueModuleName, t_xmlConfigPath, t_xmlConfigData, moduleEntityId));
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

void ModuleManager::changeSessionFile(const QString &newSessionFile)
{
    if(m_sessionFile != newSessionFile) {
        const QString sessionFileNameFull = QDir::cleanPath(QString("%1/%2").arg(m_sessionPath, newSessionFile));
        if(QFile::exists(sessionFileNameFull)) {
            if(m_moduleStartLock == false) { // do not mess up the state machines
                m_sessionFile = newSessionFile;
                destroyModules();
                emit sigSessionSwitched(sessionFileNameFull);
            }
            else {
                qWarning() << "Cannot switch sessions while session change already is in progress";
                Q_ASSERT(false);
            }
        }
        else {
            qWarning() << "Session file not found:" << sessionFileNameFull << "Search path:" << m_sessionPath;
            Q_ASSERT(false);
        }
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
    if(m_licenseSystem->serialNumberIsInitialized() == true && m_moduleStartLock == false)
    {
        onModuleStartNext();
    }
}

void ModuleManager::onModuleStartNext()
{
    m_moduleStartLock = false;
    if(m_deferredStartList.length() > 0) {
        ModuleData *tmpData = m_deferredStartList.dequeue();
        startModule(tmpData->m_uniqueName, tmpData->m_configPath, tmpData->m_configData, tmpData->m_moduleId);
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

void ModuleManager::setMockServices(QString deviceName)
{
    if (m_mockCom5003Facade) delete m_mockCom5003Facade.release();
    if (m_mockMt310s2Facade) delete m_mockMt310s2Facade.release();

    if(deviceName == "mt310s2")
        m_mockMt310s2Facade = std::make_unique<MockMt310s2Facade>();
    else if(deviceName == "com5003")
        m_mockCom5003Facade = std::make_unique<MockCom5003Facade>();
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
