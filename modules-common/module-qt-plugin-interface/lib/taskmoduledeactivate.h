#ifndef TASKMODULEDEACTIVATE_H
#define TASKMODULEDEACTIVATE_H

#include "abstractmodulefactory.h"
#include <tasktemplate.h>

namespace ZeraModules
{

class TaskModuleDeactivate : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(VirtualModule *module, AbstractModuleFactory *moduleFactory);
    explicit TaskModuleDeactivate(VirtualModule *module, AbstractModuleFactory *moduleFactory);
    void start() override;
private:
    VirtualModule *m_module;
    AbstractModuleFactory *m_moduleFactory;
};

}
#endif // TASKMODULEDEACTIVATE_H
