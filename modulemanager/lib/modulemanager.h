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
    explicit ModuleManager(const QStringList &sessionList, QObject *parent = nullptr);
    ~ModuleManager() override;
    bool loadAllAvailableModulePlugins();
    void loadScripts(VeinScript::ScriptSystem *t_scriptSystem);
    void setStorage(VeinEvent::StorageSystem *t_storage);
    void setLicenseSystem(LicenseSystemInterface *t_licenseSystem);
    void setEventHandler(ModuleEventHandler *t_eventHandler);
    void setDemo(bool demo);
signals:
    void sigSessionSwitched(const QString &newSessionFile);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    void startModule(const QString &t_uniqueName, const QString &t_xmlConfigPath, const QByteArray &t_xmlConfigData, int moduleEntityId);
    void destroyModules();
    void changeSessionFile(const QString &newSessionFile);
    void setModulesPaused(bool t_paused);

private slots:
    void onStartModuleDelete();
    void delayedModuleStartNext();
    void onModuleStartNext();
    void onModuleError(const QString &t_error);
    void checkModuleList();
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

protected:
    static QString m_sessionPath;

private:
    void saveModuleConfig(ModuleData *t_moduleData);
    virtual QStringList getModuleFileNames();

    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QList<ModuleData *> m_moduleList;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_demo;
    VeinEvent::StorageSystem *m_storage=nullptr;
    ModuleEventHandler *m_eventHandler=nullptr;
    LicenseSystemInterface *m_licenseSystem=nullptr;

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    bool m_moduleStartLock;
    QElapsedTimer m_timerAllModulesLoaded;
};
}

#endif // MODULELOADER_H
