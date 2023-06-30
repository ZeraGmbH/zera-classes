#ifndef MODULELOADER_H
#define MODULELOADER_H

#include <virtualmodule.h>
#include <QVariant>
#include <QHash>
#include <QQueue>
#include <QTimer>
#include <QElapsedTimer>


namespace VeinEvent
{
class StorageSystem;
}

namespace VeinScript
{
class ScriptSystem;
}

class ModuleEventHandler;
class MeasurementModuleFactory;
class LicenseSystem;

namespace ZeraModules
{
class ModuleData;

class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(const QStringList &t_sessionList, QObject *t_parent = nullptr);
    ~ModuleManager() override;
    /**
     * @brief loads the modules from the MODMAN_MODULE_PATH
     * @return
     */
    bool loadModules();
    /**
     * @brief loads the qml-scripts for the ScriptSystem from the qrc resource ":/qml"
     * @param t_scriptSystem
     */
    void loadScripts(VeinScript::ScriptSystem *t_scriptSystem);
    /**
     * @brief sets the storage that is passed to the modules so they can get values without fetching them all the time
     * @param t_storage
     */
    void setStorage(VeinEvent::StorageSystem *t_storage);
    /**
     * @brief sets the license system, also connects the sigSerialNumberInitialized to delayedModuleStartNext()
     * @param t_licenseSystem
     */
    void setLicenseSystem(LicenseSystem *t_licenseSystem);
    /**
     * @brief sets the ModuleEventHandler where the modules are added with addSystem()
     * @param t_eventHandler
     */
    void setEventHandler(ModuleEventHandler *t_eventHandler);

signals:
    /**
     * @brief called when the session is switched via changeSessionFile()
     * @param t_newSessionPath
     */
    void sigSessionSwitched(const QString &t_newSessionPath);
    /**
     * @brief called in onModuleStartNext() when all modules are loaded (m_deferredStartList is empty)
     * @param t_sessionPath
     * @param t_sessionsAvailable
     */
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    /**
     * @brief starts the module, or adds it to the m_deferredStartList if a startup is in progress
     * connected to the signal JsonSessionLoader::sigLoadModule , but also called in onModuleStartNext()
     * @param t_uniqueName
     * @param t_xmlConfigPath path to write back the config file if the configuration was changed
     * @param t_xmlConfigData
     * @param t_moduleId = entityId
     */
    void startModule(const QString &t_uniqueName, const QString &t_xmlConfigPath, const QByteArray &t_xmlConfigData, int t_moduleId);
    /**
     * @brief  calls the factory to delete the modules via MeasurementModuleFactory::destroyModule()
     * @warning do not confuse this with setModulesPaused()
     */
    void stopModules();
    /**
     * @brief emits sigSessionSwitched when session switching isn't already in progress
     * @param t_newSessionPath the path is appended to the MODMAN_SESSION_PATH
     */
    void changeSessionFile(const QString &t_newSessionPath);

    /**
     * @brief pauses the value component event emittion of the modules
     * @warning do not confuse this with stopModules(), the called VirtualModule::stopModule() function only pauses the module!
     * @param t_paused
     */
    void setModulesPaused(bool t_paused);

private slots:
    /**
     * @brief connected to the moduleDeactivated signal called from MeasurementModuleFactory::destroyModule()
     * deletes the ModuleData entry from the m_moduleList and connects the VirtualModule::destroyed signal to checkModuleList()
     */
    void onModuleDelete();
    /**
     * @brief calls onModuleStartNext() if the LicenseSystem::serialNumberIsInitialized() returns true
     */
    void delayedModuleStartNext();
    /**
     * @brief emits sigModulesLoaded if m_deferredStartList is empty, otherwise calls startModule() for the next entry of that list
     */
    void onModuleStartNext();
    /**
     * @brief connected to the signal VirtualModule::moduleError, just prints a warning to the journal (or whatever logging facility is set up)
     * @param t_error
     */
    void onModuleError(const QString &t_error);
    /**
     * @brief when switching sessions during runtime the old modules need to be destroyed before the new ones can be created
     * this function checks if the m_moduleList is empty and calls onModuleStartNext() in this case
     */
    void checkModuleList();

    /**
     * @brief calls ModuleEventHandler::addSystem();
     * @todo the connect in startModule() can be replaced with a direct connection to ModuleEventHandler::addSystem() and this intermediate can be removed
     * @param t_eventSystem
     */
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

private:
    /**
     * @brief writes the changed configuration file of a module back to disk
     * @param t_moduleData
     */
    void saveModuleConfig(ModuleData *t_moduleData);

    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QList<ModuleData *> m_moduleList;
    QQueue<ModuleData *> m_deferredStartList;

    VeinEvent::StorageSystem *m_storage=nullptr;
    ModuleEventHandler *m_eventHandler=nullptr;
    LicenseSystem *m_licenseSystem=nullptr;

    QString m_sessionPath;
    QList<QString> m_sessionsAvailable;

    bool m_moduleStartLock;
    QElapsedTimer m_timerAllModulesLoaded;
};
}

#endif // MODULELOADER_H
