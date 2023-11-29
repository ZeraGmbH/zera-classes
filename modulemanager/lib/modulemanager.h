#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "jsonsessionloader.h"
#include "modulemanagersetupfacade.h"
#include "mockmt310s2facade.h"
#include "mockcom5003facade.h"
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
    bool areAllModulesShutdown();
    void setupConnections();
    void loadDefaultSession();
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
    void checkModuleList(QObject *object);
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);

protected:
    void setMockServices(QString deviceName);

    static QString m_sessionPath;
    static bool m_runningInTest;

private:
    void saveModuleConfig(ModuleData *t_moduleData);
    virtual QStringList getModuleFileNames();

    ModuleManagerSetupFacade *m_setupFacade;
    JsonSessionLoader m_sessionLoader;
    QHash<QString, MeasurementModuleFactory*> m_factoryTable;
    QList<ModuleData *> m_moduleList;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_demo;

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    bool m_moduleStartLock;
    QElapsedTimer m_timerAllModulesLoaded;

    std::unique_ptr<MockMt310s2Facade> m_mockMt310s2Facade;
    std::unique_ptr<MockCom5003Facade> m_mockCom5003Facade;
};
}

#endif // MODULELOADER_H
