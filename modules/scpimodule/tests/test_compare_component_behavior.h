#ifndef TEST_COMPARE_COMPONENT_BEHAVIOR_H
#define TEST_COMPARE_COMPONENT_BEHAVIOR_H

#include "modulemanagertestrunner.h"
#include <testjsonspyeventsystem.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>

class test_compare_component_behavior : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    void queryOtherEntity();
    void querySystemEntity();
    void cmdOtherEntity();
    void cmdSystemEntity();


private:
    void setupSpy();


    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;

    QJsonObject m_veinEventDump;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyBottom;
};

#endif // TEST_COMPARE_COMPONENT_BEHAVIOR_H
