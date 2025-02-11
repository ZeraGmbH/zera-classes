#ifndef MODULELOADER_H
#define MODULELOADER_H

#include "abstractfactoryserviceinterfaces.h"
#include "jsonsessionloader.h"
#include "modulefactoryparam.h"
#include "modulemanagersetupfacade.h"
#include "moduledata.h"
#include "abstractmockallservices.h"
#include <abstracttcpnetworkfactory.h>
#include <virtualmodule.h>
#include <QVariant>
#include <QHash>
#include <QQueue>
#include <QTimer>
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
    explicit ModuleManager(ModuleManagerSetupFacade *setupFacade,
                           AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           bool moduleDemoMode = false,
                           QObject *parent = nullptr);
    ~ModuleManager() override;
    bool loadAllAvailableModulePlugins();
    void setupConnections();
    void loadDefaultSession();
    void startAllDemoServices(QString deviceName);
signals:
    void sigSessionSwitched(const QString &newSessionFile);
    void sigModulesLoaded(const QString &t_sessionPath, const QStringList &t_sessionsAvailable);

public slots:
    bool loadSession(const QString sessionFileNameFull);
    void changeSessionFile(const QString &newSessionFile);
    void startModule(const QString &uniqueName,
                     const QString &xmlConfigPath,
                     int moduleEntityId,
                     int moduleNum);
    void destroyModules();
    void setModulesPaused(bool t_paused);

private slots:
    void onStartModuleDelete();
    void delayedModuleStartNext();
    void onModuleStartNext();
    void onDestroyModule(QObject *object);
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
                       int moduleNum);

    std::shared_ptr<ModuleSharedData> m_moduleSharedObjects;
    ModuleManagerSetupFacade *m_setupFacade;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    JsonSessionLoader m_sessionLoader;
    QHash<QString, AbstractModuleFactory*> m_factoryTable;
    QQueue<ModuleData *> m_deferredStartList;

    bool m_moduleDemoMode; // To be obsoleted: Eiter run full production or inject a test/demo service factory

    QString m_sessionFile;
    QList<QString> m_sessionsAvailable;

    QElapsedTimer m_timerAllModulesLoaded;

};
}

#endif // MODULELOADER_H
