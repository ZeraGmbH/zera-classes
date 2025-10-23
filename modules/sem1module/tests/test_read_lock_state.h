#ifndef TEST_READ_LOCK_STATE_H
#define TEST_READ_LOCK_STATE_H

#include <QObject>

class test_read_lock_state : public QObject
{
    Q_OBJECT
private slots:
    void readEmobPushButtonValue();
    void pressAndReadEmobPushButtonValue();
    void readLockStateWrongRpcName();
    void readLockStateCorrectRpcName();
    void readLockStateTwice();
};

#endif // TEST_READ_LOCK_STATE_H
