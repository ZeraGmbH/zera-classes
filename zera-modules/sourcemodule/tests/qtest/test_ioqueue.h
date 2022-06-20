#ifndef IOQUEUETEST_H
#define IOQUEUETEST_H

#include <QObject>
#include "io-queue/ioqueue.h"

class test_ioqueue : public QObject
{
    Q_OBJECT
public slots:
    void onIoQueueGroupFinished(IoQueueEntry::Ptr workGroup);
private slots:
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
    static IoQueueEntry::Ptr generateSwitchCommands(bool on);
    static IoQueueEntry::Ptr generateStatusPollCommands();
    void evalNotificationCount(int passedGroupsExpected,
                               int passExpected, int failExpected, int unknownExpected);

    IoQueueEntryList m_listIoGroupsReceived;
};

#endif // IOQUEUETEST_H
