#ifndef IOQUEUETEST_H
#define IOQUEUETEST_H

#include <QObject>
#include "io-queue/ioqueue.h"

class IoQueueTest : public QObject
{
    Q_OBJECT
public slots:
    void onIoQueueGroupFinished(IoTransferDataGroup workGroup);
private slots:
    void init();

    void noIoDeviceNotBusy();
    void notOpenIoDeviceNotBusy();
    void emptyGroupNotBusy();
    void openIoDeviceBusy();

    void noIoDeviceNotification();
    void notOpenIoDeviceNotifications();

    void disconnectBeforeEnqueue();
    void disconnectWhileWorking();
    void disconnectWhileWorkingMultipleNotifications();

    void stopOnFirstError();
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
    static IoTransferDataGroup generateSwitchCommands(bool on);
    static IoTransferDataGroup generateStatusPollCommands();
    void evalNotificationCount(int passedGroupsExpected,
                               int passExpected, int failExpected, int unknownExpected);

    QList<IoTransferDataGroup> m_listIoGroupsReceived;
};

#endif // IOQUEUETEST_H
