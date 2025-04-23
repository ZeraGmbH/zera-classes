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

TaskTemplatePtr TaskAllModulesDestroy::create(std::unique_ptr<QList<ModuleData *>> moduledataList,
                                              const QHash<QString, AbstractModuleFactory*> &factoryTable,
                                              const ModuleNetworkParamsPtr &networkParams)
{
    return std::make_unique<TaskAllModulesDestroy>(std::move(moduledataList), factoryTable, networkParams);
}

TaskAllModulesDestroy::TaskAllModulesDestroy(std::unique_ptr<QList<ModuleData *>> moduledataList,
                                             const QHash<QString, AbstractModuleFactory *> &factoryTable,
                                             const ModuleNetworkParamsPtr &networkParams) :
    m_moduledataList(std::move(moduledataList)),
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
    cleanupModuleData();
}

void TaskAllModulesDestroy::cleanupModuleData()
{
    for(ModuleData *moduleData : *m_moduledataList)
        delete moduleData;
    m_moduledataList.reset();
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
    for (int entry=m_moduledataList->count()-1; entry>=0; entry--) { // reversed order is intended here
        const ModuleData* moduleData = m_moduledataList->at(entry);
        TaskContainerInterfacePtr tasksModule = TaskContainerSequence::create();
        AbstractModuleFactory* factory = m_factoryTable[moduleData->m_uniqueName];

        tasksModule->addSub(TaskModuleDeactivate::create(moduleData->m_module, factory));
        tasksModule->addSub(TaskModuleDelayedDelete::create(moduleData->m_module));

        tasks->addSub(std::move(tasksModule));
    }
    return tasks;
}

}
