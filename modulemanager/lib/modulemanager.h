#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "abstractfactoryserviceinterfaces.h"
#include "jsonsessionloader.h"
#include "modulemanagersetupfacade.h"
#include "moduledata.h"
#include "modulenetworkparams.h"
#include "moduleshareddata.h"
#include <abstracttcpnetworkfactory.h>
#include <virtualmodule.h>
#include <QVariant>
#include <QHash>
#include <QQueue>
#include <QElapsedTimer>

namespace VeinEvent
{
class AbstractEventSystem;
}

class AbstractModuleFactory;

namespace ZeraModules
{
class ModuleManager : public QObject
{
    Q_OBJECT
public:
    static QString getInstalledSessionPath();
    explicit ModuleManager(ModuleManagerSetupFacade *setupFacade,
                           AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           bool moduleDemoMode = false,
                           QObject *parent = nullptr);
    ~ModuleManager() override;
    bool loadAllAvailableModulePlugins();
    void setupConnections();
    void loadDefaultSession();
signals:
    void sigSessionSwitched(const QString &newSessionFile);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);
    void sigModulesUnloaded();

public slots:
    bool loadSession(const QString &sessionFileNameFull);
    void changeSessionFile(const QString &newSessionFile);
    void startModule(const QString &uniqueName,
                     const QString &xmlConfigPath,
                     int moduleEntityId,
                     int moduleNum,
                     bool licensable);
    void destroyModules();
    void setModulesPaused(bool t_paused);

private slots:
    void onAllModulesDestroyed();

    void delayedModuleStartNext();
    void onModuleStartNext();
    void onModuleEventSystemAdded(VeinEvent::EventSystem *t_eventSystem);
    void onEventSubSystemAdded(VeinEvent::EventSystem *t_eventSystem);
    void onEventSubSystemRemoved(VeinEvent::EventSystem *t_eventSystem);

protected:
    static QString m_sessionPath;
    std::unique_ptr<QList<ModuleData *>> m_moduleDataList;
    bool m_moduleStartLock;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;

private:
    virtual void saveModuleConfig(ModuleData *moduleData);
    virtual void saveDefaultSession();
    virtual QStringList getModuleFileNames();
    static std::unique_ptr<QList<ModuleData *>> createEmptyModuleDataList();
    ModuleNetworkParamsPtr getNetworkParams();
    void handleFinalModuleLoaded();
    void createCommonModuleParam();
    VirtualModule *createModule(const QString &xmlConfigPath,
                                int moduleEntityId,
                                int moduleNum,
                                const QString &uniqueName,
                                AbstractModuleFactory *tmpFactory);
    void doStartModule(VirtualModule *tmpModule,
                       const QString &uniqueName,
                       const QString &xmlConfigPath,
                       int moduleEntityId,
                       int moduleNum,
                       bool licensable);
    void disconnectModulesFromVein();

    std::shared_ptr<ModuleSharedData> m_moduleSharedObjects;
    ModuleManagerSetupFacade *m_setupFacade;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    JsonSessionLoader m_sessionLoader;
    QHash<QString, AbstractModuleFactory*> m_factoryTable;
    QQueue<ModuleData *> m_deferredStartList;
    TaskTemplatePtr m_currModulePrepareTask;
    TaskTemplatePtr m_allModulesDestroyTask;

    bool m_moduleDemoMode; // To be obsoleted: Eiter run full production or inject a test/demo service factory

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    QElapsedTimer m_timerAllModulesLoaded;
};
}

#endif // MODULELOADER_H
