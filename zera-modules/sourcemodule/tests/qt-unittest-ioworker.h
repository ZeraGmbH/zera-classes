#ifndef SOURCEIOWORKERENTRYTEST_H
#define SOURCEIOWORKERENTRYTEST_H

#include <QObject>
#include "io-worker/ioworker.h"

class IoWorkerTest : public QObject
{
    Q_OBJECT
public slots:
    void onWorkPackFinished(IoWorkerCmdPack workPack);
private slots:
    void init();

    void emptyWorkerIsInvalid();
    void cmdToWorkProperties();
    void cmdPackToWorkIoSize();
    void cmdPackToWorkIoSequence();

    void noInterfaceNotBusy();
    void notOpenInterfaceNotBusy();
    void emptyPackNotBusy();
    void openInterfaceBusy();

    void noInterfaceNotification();
    void notOpenInterfaceNotifications();

    void disconnectBeforeEnqueue();
    void disconnectWhileWorking();
    void disconnectWhileWorkingMultipleNotifications();

    void testStopOnFirstError();
    void testContinueOnError();

    void noErrorSigOnEmptyPack();

    void testSpamRejected();
    void testCloseToSpamAccepted();

    void testOnePacketSingleIoOK();
    void testTwoPacketSingleIoOK();
    void testOnePacketMultipleIoOK();
    void testTwoPacketMultipleIoOK();

    void timeoutDetected();

private:
    static tIoInterfaceShPtr createOpenInterface();
    static IoWorkerCmdPack generateSwitchCommands(bool on);
    static IoWorkerCmdPack generateStatusPollCommands();
    static void adjustWorkCmdPack(IoWorkerCmdPack& workCmdPack,
                                  PacketErrorBehaviors errorBehavior);
    void evalNotificationCount(int cmdPassedExpected, int passExpected, int failExpected, int unknownExpected);

    QList<IoWorkerCmdPack> m_listWorkPacksReceived;
};

#endif // SOURCEIOWORKERENTRYTEST_H
