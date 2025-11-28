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
    readData,
    writeData
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
    void controllersFound(QVariant value);
private:
    void addedRemovedControllers(bool ctrlersFound);

    cHotplugControlsModule *m_module;
    PcbServiceConnection m_pcbConnection;
    TaskContainerSequence m_activationTasks;
    QHash<quint32, int> m_MsgNrCmdList;

    VfModuleRpcPtr m_pEmobPushButtonRpc;
    VfModuleRpcPtr m_pEmobLockStateRpc;
    VfModuleParameter *m_pControllersFound;
};

}

#endif // HOTPLUGCONTROLSMODULECONTROLLER_H
