#ifndef SOURCEIOWORKERENTRYTEST_H
#define SOURCEIOWORKERENTRYTEST_H

#include <QObject>
#include "io-worker/ioworker.h"

class IoWorkerTest : public QObject
{
    Q_OBJECT
public slots:
    void onIoWorkerGroupFinished(IoMultipleTransferGroup workGroup);
private slots:
    void init();

    void emptyWorkerIsInvalid();
    void mulipleTransferIniitialProperties();

    void noInterfaceNotBusy();
    void notOpenInterfaceNotBusy();
    void emptyGroupNotBusy();
    void openInterfaceBusy();

    void noInterfaceNotification();
    void notOpenInterfaceNotifications();

    void disconnectBeforeEnqueue();
    void disconnectWhileWorking();
    void disconnectWhileWorkingMultipleNotifications();

    void stopOnFirstError();
    void continueOnError();

    void noErrorSigalOnEmptyGroup();

    void rejectSpam();
    void acceptCloseToSpam();

    void oneValidGroupSingleIo();
    void twoValidGroupsSingleIo();
    void oneValidGroupMultipleIo();
    void twoValidGroupsMultipleIo();
    void twoFirstInvalidSecondOkSingleIo();

    void timeoutDetected();

private:
    static tIoInterfaceShPtr createOpenInterface();
    static IoMultipleTransferGroup generateSwitchCommands(bool on);
    static IoMultipleTransferGroup generateStatusPollCommands();
    static void adjustWorkTransferGroup(IoMultipleTransferGroup& workTransferGroup,
                                  GroupErrorBehaviors errorBehavior);
    void evalNotificationCount(int passedGroupsExpected,
                               int passExpected, int failExpected, int unknownExpected);

    QList<IoMultipleTransferGroup> m_listIoGroupsReceived;
};

#endif // SOURCEIOWORKERENTRYTEST_H
