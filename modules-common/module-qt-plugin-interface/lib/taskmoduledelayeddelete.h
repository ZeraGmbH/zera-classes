#ifndef TASKMODULEDELAYEDDELETE_H
#define TASKMODULEDELAYEDDELETE_H

#include "virtualmodule.h"
#include <tasktemplate.h>

namespace ZeraModules
{

class TaskModuleDelayedDelete : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(VirtualModule *module);
    explicit TaskModuleDelayedDelete(VirtualModule *module);
    void start() override;
private:
    VirtualModule *m_module;
    int m_entityId = 0;
    QString m_veinModuleName;
};

}
#endif // TASKMODULEDELAYEDDELETE_H
