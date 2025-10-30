#ifndef TEST_READ_LOCK_STATE_H
#define TEST_READ_LOCK_STATE_H

#include "modulemanagertestrunner.h"
#include <testjsonspyeventsystem.h>
#include <QObject>
#include <QJsonObject>

class test_read_lock_state : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();
    void readEmobPushButtonValue();
    void pressAndReadEmobPushButtonValue();
    void readLockStateWrongRpcName();
    void readLockStateCorrectRpcName();
    void readLockStateTwice();
private:
    void setupSpy(ModuleManagerTestRunner &modmanRunner);

    QJsonObject m_veinEventDump;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyTop;
    std::unique_ptr<TestJsonSpyEventSystem> m_serverCmdEventSpyBottom;
};

#endif // TEST_READ_LOCK_STATE_H
