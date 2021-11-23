#ifndef SOURCEIOWORKERENTRYTEST_H
#define SOURCEIOWORKERENTRYTEST_H

#include <QObject>
#include "sourceioworker.h"

class SourceIoWorkerTest : public QObject
{
    Q_OBJECT
public slots:
    void onWorkPackFinished(cWorkerCommandPacket workPack);
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

    void testStopOnFirstErrorFullResponse();
    void testStopOnFirstErrorPartResponse();
    void testContinueOnErrorFullResponse();
    void testContinueOnErrorPartResponse();

    void testSpamRejected();
    void testCloseToSpamAccepted();

    void testOnePacketSingleIoOK();
    void testTwoPacketSingleIoOK();
    void testOnePacketMultipleIoOK();
    void testTwoPacketMultipleIoOK();

private:
    static tSourceInterfaceShPtr createOpenInterface();
    static cWorkerCommandPacket generateSwitchCommands(bool on);
    static cWorkerCommandPacket generateStatusPollCommands();
    static void adjustWorkCmdPack(cWorkerCommandPacket& workCmdPack,
                                  SourcePacketErrorBehaviors errorBehavior,
                                  SourceIoResponseTypes responseType);
    void evalNotificationCount(int cmdPassedExpected, int passExpected, int failExpected, int unknownExpected);

    QList<cWorkerCommandPacket> m_listWorkPacksReceived;
};

#endif // SOURCEIOWORKERENTRYTEST_H
