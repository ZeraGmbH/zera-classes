#ifndef HOTPLUGCONTROLSMODULECONTROLLER_H
#define HOTPLUGCONTROLSMODULECONTROLLER_H

#include "hotplugcontrolsmodule.h"
#include "moduleactivist.h"

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
    void onActivationReady();
    void onDeactivationReady();
private:
    cHotplugControlsModule *m_module;
};

}

#endif // HOTPLUGCONTROLSMODULECONTROLLER_H
