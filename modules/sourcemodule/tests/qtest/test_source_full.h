#ifndef TEST_SOURCE_FULL_H
#define TEST_SOURCE_FULL_H

#include "modulemanagertestrunner.h"
#include "vf_core_stack_client.h"
#include "vf_entity_component_event_item.h"
#include "testjsonspyeventsystem.h"
#include <QJsonObject>
#include <memory>

class test_source_full : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void entity_avail_mt310s2();
    void entity_avail_mt581s2();
    void vein_dump_mt310s2();
    void vein_dump_mt581s2();
    void switch_on_mt310s2();
private:
    void setupServerAndClient(const QString &session, const QString &dut);
    void setupServerAndClientSpies(QJsonObject &jsonEvents);
    void setDemoSourceCount(int sourceCount);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinNet::NetworkSystem> m_serverNetSystem;
    std::unique_ptr<VeinNet::TcpSystem> m_serverTcpSystem;

    std::unique_ptr<VfCoreStackClient> m_netClient;
    VfCmdEventItemEntityPtr m_entityItem;
    std::unique_ptr<TestJsonSpyEventSystem> m_clientCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyBottom;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_clientCmdEventSpyBottom;
};

#endif // TEST_SOURCE_FULL_H
