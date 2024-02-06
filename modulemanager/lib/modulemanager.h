#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "jsonsessionloader.h"
#include "modulemanagersetupfacade.h"
#include "moduledata.h"
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
class ModuleManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleManager(ModuleManagerSetupFacade *setupFacade, bool demo = false, QObject *parent = nullptr);
    ~ModuleManager() override;
    bool loadAllAvailableModulePlugins();
    void loadScripts(VeinScript::ScriptSystem *t_scriptSystem);
    void setupConnections();
    void loadDefaultSession();
    virtual void startAllServiceMocks(QString deviceName);
signals:
    void sigSessionSwitched(const QString &newSessionFile);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    bool loadSession(const QString sessionFileNameFull);
    void changeSessionFile(const QString &newSessionFile);
    void startModule(const QString &t_uniqueName, const QString &t_xmlConfigPath, const QByteArray &t_xmlConfigData, int moduleEntityId, int moduleNum);
    void destroyModules();
    void setModulesPaused(bool t_paused);

private slots:
    void onStartModuleDelete();
    void delayedModuleStartNext();
    void onModuleStartNext();
    void checkModuleList(QObject *object);
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

protected:
    static QString m_sessionPath;
    QList<ModuleData *> m_moduleList;
    bool m_moduleStartLock;
    std::unique_ptr<AbstractMockAllServices> m_mockAllServices;

private:
    virtual void saveModuleConfig(ModuleData *moduleData);
    virtual void saveDefaultSession();
    virtual QStringList getModuleFileNames();
    void handleFinalModuleLoaded();

    ModuleManagerSetupFacade *m_setupFacade;
    JsonSessionLoader m_sessionLoader;
    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_demo;

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    QElapsedTimer m_timerAllModulesLoaded;

};
}

#endif // MODULELOADER_H
