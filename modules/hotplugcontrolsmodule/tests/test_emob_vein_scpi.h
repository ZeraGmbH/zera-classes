#ifndef TEST_EMOB_VEIN_SCPI_H
#define TEST_EMOB_VEIN_SCPI_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <testjsonspyeventsystem.h>
#include <vf_core_stack_client.h>
#include <QObject>
#include <QJsonObject>
#include <vf_rpc_invoker.h>

class test_emob_vein_scpi : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void init();
    void invokeInvalidRpcNameScpi();

    void activateEmobPushButtonNoEmobNoParamScpi();
    void activateEmobPushNoParamOneEmob();
    void activateEmobPushButtonWithParamScpi();
    void activateEmobPushNoParamMutipleHotplug();
    void activateEmobPushInvalidParam();
    void activateEmobPushIncorrectParam();
    void activateEmobPushValidParamMutipleHotplug();
    void activateEmobPushButtonVein();

    void readLockStateCorrectRpcNameScpi();
    void readLockStateScpiNoParamOneEmob();
    void readLockStateScpiNoParamMutipleHotplug();
    void readLockStateScpiInvalidParam();
    void readLockStateScpiIncorrectParam();
    void readLockStateScpiValidParamMutipleHotplug();

    void readLockStateTwiceScpi();
    void readLockStateTwiceVein();
    void readLockStateTwiceVeinFullQueue();

    void dumpDevIface();
    void dumpVeinInfModuleInterface();
private:
    void setupSpy();
    void invokeRpc(QString rpcName, QString paramName, QVariant paramValue);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinNet::NetworkSystem> m_netSystem;
    std::unique_ptr<VeinNet::TcpSystem> m_tcpSystem;

    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
    std::unique_ptr<VfCoreStackClient> m_veinClientStack;
    VfRPCInvokerPtr m_rpcInvoker;

    QJsonObject m_veinEventDump;
    std::unique_ptr<TestJsonSpyEventSystem> m_clientCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_clientCmdEventSpyBottom;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyBottom;
};

#endif // TEST_EMOB_VEIN_SCPI_H
