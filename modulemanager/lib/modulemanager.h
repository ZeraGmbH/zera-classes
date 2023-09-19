#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "licensesysteminterface.h"
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

namespace ZeraModules
{
class ModuleData;

class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(const QStringList &t_sessionList, QObject *t_parent = nullptr);
    ~ModuleManager() override;
    bool loadModules();
    void loadScripts(VeinScript::ScriptSystem *t_scriptSystem);
    void setStorage(VeinEvent::StorageSystem *t_storage);
    void setLicenseSystem(LicenseSystemInterface *t_licenseSystem);
    void setEventHandler(ModuleEventHandler *t_eventHandler);
    void setDemo(bool demo);
signals:
    void sigSessionSwitched(const QString &t_newSessionPath);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    void startModule(const QString &t_uniqueName, const QString &t_xmlConfigPath, const QByteArray &t_xmlConfigData, int t_moduleId);
    void destroyModules();
    void changeSessionFile(const QString &t_newSessionPath);
    void setModulesPaused(bool t_paused);

private slots:
    void onModuleDelete();
    void delayedModuleStartNext();
    void onModuleStartNext();
    void onModuleError(const QString &t_error);
    void checkModuleList();
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

private:
    void saveModuleConfig(ModuleData *t_moduleData);

    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QList<ModuleData *> m_moduleList;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_demo;
    VeinEvent::StorageSystem *m_storage=nullptr;
    ModuleEventHandler *m_eventHandler=nullptr;
    LicenseSystemInterface *m_licenseSystem=nullptr;

    QString m_sessionPath;
    QList<QString> m_sessionsAvailable;

    bool m_moduleStartLock;
    QElapsedTimer m_timerAllModulesLoaded;
};
}

#endif // MODULELOADER_H
