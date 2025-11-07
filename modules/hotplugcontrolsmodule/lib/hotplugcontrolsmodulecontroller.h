#ifndef HOTPLUGCONTROLSMODULECONTROLLER_H
#define HOTPLUGCONTROLSMODULECONTROLLER_H

#include "hotplugcontrolsmodule.h"
#include "moduleactivist.h"
#include "pcbserviceconnection.h"
#include <taskcontainersequence.h>

namespace  HOTPLUGCONTROLSMODULE {

enum hotplugControlsModuleCmds
{
    activatepushbutton,
};

class HotplugControlsModuleController : public cModuleActivist
{
public:
    HotplugControlsModuleController(cHotplugControlsModule *module);
public slots:
    void activate() override;
    void deactivate() override;
    void generateVeinInterface() override;
private slots:
    void onActivateDone(bool ok);
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

    void newPushButton(QVariant pushbutton);
private:
    cHotplugControlsModule *m_module;
    PcbServiceConnection m_pcbConnection;
    TaskContainerSequence m_activationTasks;
    QHash<quint32, int> m_MsgNrCmdList;

    VfModuleParameter* m_pPressPushButton;
    VfModuleRpcPtr m_pEmobLockStateRpc;
};

}

#endif // HOTPLUGCONTROLSMODULECONTROLLER_H
