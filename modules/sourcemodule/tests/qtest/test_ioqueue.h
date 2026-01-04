#ifndef IOQUEUETEST_H
#define IOQUEUETEST_H

#include <QObject>
#include "ioqueue.h"

class test_ioqueue : public QObject
{
    Q_OBJECT
public slots:
    void onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup);
private slots:
    void initTestCase();
    void init();

    void noIoDeviceNotBusy();
    void notOpenIoDeviceNotBusy();
    void emptyGroupNotBusy();
    void nullGroupNotBusy();
    void openIoDeviceBusy();

    void noIoDeviceNotification();
    void notOpenIoDeviceNotifications();

    void disconnectBeforeEnqueue();
    void disconnectWhileWorking();
    void disconnectWhileWorkingMultipleNotifications();

    void stopOnFirstError();
    void stopOnFirstOk();
    void continueOnError();

    void noErrorSignalOnEmptyGroup();

    void rejectSpam();
    void acceptCloseToSpam();

    void oneValidGroupSingleIo();
    void twoValidGroupsSingleIo();
    void oneValidGroupMultipleIo();
    void twoValidGroupsMultipleIo();
    void twoFirstInvalidSecondOkSingleIo();

private:
    static IoQueueGroup::Ptr generateSwitchCommands(bool on);
    static IoQueueGroup::Ptr generateStatusPollCommands();
    void evalNotificationCount(int passedGroupsExpected,
                               int passExpected, int failExpected, int unknownExpected);

    IoQueueGroupListPtr m_listIoGroupsReceived;
};

#endif // IOQUEUETEST_H
