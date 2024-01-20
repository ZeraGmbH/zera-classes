#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "jsonsessionloader.h"
#include "modulemanagersetupfacade.h"
#include "abstractmockallservices.h"
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

class MeasurementModuleFactory;

namespace ZeraModules
{
class ModuleData;

class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    ~ModuleManager() override;
    bool loadAllAvailableModulePlugins();
    void loadScripts(VeinScript::ScriptSystem *t_scriptSystem);
    bool modulesReady();
    void setupConnections();
    void loadDefaultSession();
    void setDemoServices(QString deviceName);
signals:
    void sigSessionSwitched(const QString &newSessionFile);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    void startModule(const QString &t_uniqueName, const QString &t_xmlConfigPath, const QByteArray &t_xmlConfigData, int moduleEntityId, int moduleNum);
    void destroyModules();
    void changeSessionFile(const QString &newSessionFile);
    void setModulesPaused(bool t_paused);

private slots:
    void onStartModuleDelete();
    void delayedModuleStartNext();
    void onModuleStartNext();
    void onModuleError(const QString &t_error);
    void checkModuleList(QObject *object);
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

protected:
    static QString m_sessionPath;
    static bool m_runningInTest;
    QList<ModuleData *> m_moduleList;
    std::unique_ptr<AbstractMockAllServices> m_mockAllServices;

    bool loadSession(const QString sessionFileNameFull);

private:
    void saveModuleConfig(ModuleData *t_moduleData);
    virtual QStringList getModuleFileNames();

    ModuleManagerSetupFacade *m_setupFacade;
    JsonSessionLoader m_sessionLoader;
    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_demo;

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    bool m_moduleStartLock;
    QElapsedTimer m_timerAllModulesLoaded;

};
}

#endif // MODULELOADER_H
