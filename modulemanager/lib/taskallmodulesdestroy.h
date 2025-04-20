#ifndef TASKALLMODULESDESTROY_H
#define TASKALLMODULESDESTROY_H

#include "abstractmodulefactory.h"
#include "moduledata.h"
#include "modulenetworkparams.h"
#include <taskcontainerinterface.h>
#include <QElapsedTimer>

namespace ZeraModules
{
class TaskAllModulesDestroy : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(std::unique_ptr<QList<ModuleData *>> modules,
                                  const QHash<QString, AbstractModuleFactory*> &factoryTable,
                                  const ModuleNetworkParamsPtr &networkParams);
    TaskAllModulesDestroy(std::unique_ptr<QList<ModuleData *>> modules,
                          const QHash<QString, AbstractModuleFactory*> &factoryTable,
                          const ModuleNetworkParamsPtr &networkParams);
    void start() override;

private slots:
    void onFinish(bool ok);
private:
    TaskContainerInterfacePtr createTasks();
    TaskContainerInterfacePtr createModuleDestroyTasks();
    void cleanupModuleData();

    std::unique_ptr<QList<ModuleData *>> m_modules;
    const QHash<QString, AbstractModuleFactory*> m_factoryTable;
    const ModuleNetworkParamsPtr m_networkParams;
    TaskContainerInterfacePtr m_tasks;
    QElapsedTimer m_timerDuration;
};
}

#endif // TASKALLMODULESDESTROY_H
