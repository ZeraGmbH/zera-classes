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
    void controllersFound(QVariant value);
private:
    void readWriteData(const QString &emobChannelAlias, const QString &mt650eChannelAlias);
    int getEmobId(const QString &emobChannelAlias);

    cHotplugControlsModule *m_module;
    PcbServiceConnection m_pcbConnection;
    TaskContainerSequence m_activationTasks;
    QHash<quint32, int> m_MsgNrCmdList;
    ChannelRangeObserver::SystemObserverPtr m_observer;

    VfModuleRpcPtr m_pEmobPushButtonRpc;
    VfModuleRpcPtr m_pEmobLockStateRpc;
    VfModuleRpcPtr m_pEmobReadErrorRpc;
    VfModuleRpcPtr m_pEmobClearErrorRpc;
    VfModuleRpcPtr m_pEmobFlipSwitchRpcOn;
    VfModuleRpcPtr m_pEmobFlipSwitchRpcOff;
    VfModuleParameter *m_pControllersFound;
    TaskContainerInterfacePtr m_readWriteTasks;
};

}

#endif // HOTPLUGCONTROLSMODULECONTROLLER_H
