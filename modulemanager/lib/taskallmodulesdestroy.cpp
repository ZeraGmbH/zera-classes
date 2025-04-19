#include "taskallmodulesdestroy.h"
#include "taskserverconnectionstart.h"
#include "taskremovecmdlistsforallclients.h"
#include "taskmoduledeactivate.h"
#include "taskmoduledelayeddelete.h"
#include <abstractserverInterface.h>
#include <dspinterface.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <proxy.h>

namespace ZeraModules
{

TaskTemplatePtr TaskAllModulesDestroy::create(const QList<ModuleData *> &modules,
                                              const QHash<QString, AbstractModuleFactory*> &factoryTable,
                                              const ModuleNetworkParamsPtr &networkParams)
{
    return std::make_unique<TaskAllModulesDestroy>(modules, factoryTable, networkParams);
}

TaskAllModulesDestroy::TaskAllModulesDestroy(const QList<ModuleData *> &modules,
                                             const QHash<QString, AbstractModuleFactory *> &factoryTable,
                                             const ModuleNetworkParamsPtr &networkParams) :
    m_modules(modules),
    m_factoryTable(factoryTable),
    m_networkParams(networkParams)
{
}

void TaskAllModulesDestroy::start()
{
    qInfo("Start destroying all modules...");
    m_timerDuration.start();
    m_tasks = createTasks();
    connect(m_tasks.get(), &TaskTemplate::sigFinish, this, &TaskAllModulesDestroy::onFinish);
    m_tasks->start();
}

void TaskAllModulesDestroy::onFinish(bool ok)
{
    if(ok)
        qInfo("All modules destroyed within within %llims", m_timerDuration.elapsed());
    else
        qCritical("An error occured destroying all modules!");
    finishTask(ok);
}

TaskContainerInterfacePtr TaskAllModulesDestroy::createTasks()
{
    TaskContainerInterfacePtr tasks = TaskContainerSequence::create();
    const Zera::ProxyClientPtr client = Zera::Proxy::getInstance()->getConnectionSmart(m_networkParams->m_dspServiceConnectionInfo,
                                                                                       m_networkParams->m_tcpNetworkFactory);
    tasks->addSub(TaskServerConnectionStart::create(client, CONNECTION_TIMEOUT));

    Zera::DspInterfacePtr dspInterface = std::make_shared<Zera::cDSPInterface>();
    dspInterface->setClientSmart(client);
    tasks->addSub(TaskRemoveCmdListsForAllClients::create(dspInterface, TRANSACTION_TIMEOUT,
                                                          []{ qCritical("Remove all DSP command lists failed!"); }));
    tasks->addSub(createModuleDestroyTasks());
    return tasks;
}

TaskContainerInterfacePtr TaskAllModulesDestroy::createModuleDestroyTasks()
{
    TaskContainerInterfacePtr tasks = TaskContainerParallel::create();
    for (ModuleData* moduleData : m_modules) { // TaskContainerParallel reverses order which is intended here
        TaskContainerInterfacePtr tasksModule = TaskContainerSequence::create();
        AbstractModuleFactory* factory = m_factoryTable[moduleData->m_uniqueName];
        tasksModule->addSub(TaskModuleDeactivate::create(moduleData->m_module, factory));

        // Insane utter obscurity at its best:
        // Delayed delete (deleteLater) is necessary due to nasty state machine layout in modules:
        // Module activist's state machine emits signal 'deactivated' which is DIRECT connected
        // to module's 'deactivationContinue' signal which progresses state machine and fires
        // 'deactivationReady' which in turn is direct connected to 'moduleDeactivated'.
        // Long story short: module's deactivation state machine fires before module activist's
        // state machine is finished. So work around by delayed delete.
        tasksModule->addSub(TaskModuleDelayedDelete::create(moduleData->m_module));
        tasks->addSub(std::move(tasksModule));
    }
    return tasks;
}

}
