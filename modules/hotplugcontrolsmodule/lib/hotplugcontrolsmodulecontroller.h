#ifndef HOTPLUGCONTROLSMODULECONTROLLER_H
#define HOTPLUGCONTROLSMODULECONTROLLER_H

#include "hotplugcontrolsmodule.h"
#include "moduleactivist.h"
#include "pcbserviceconnection.h"
#include <taskcontainersequence.h>

namespace  HOTPLUGCONTROLSMODULE {

class HotplugControlsModuleController : public cModuleActivist
{
public:
    HotplugControlsModuleController(cHotplugControlsModule *module);
public slots:
    void activate() override;
    void deactivate() override;
    void generateVeinInterface() override;
private slots:
    void onActivateContinue(bool ok);
private:
    cHotplugControlsModule *m_module;
    PcbServiceConnection m_pcbConnection;
    TaskContainerSequence m_activationTasks;

    VfModuleRpcPtr m_pEmobLockStateRpc;
};

}

#endif // HOTPLUGCONTROLSMODULECONTROLLER_H
