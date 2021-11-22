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

    void testEmptyWorkerIsInvalid();
    void testCmdPackToWorkType();
    void testCmdPackToWorkIoSize();
    void testCmdPackToWorkIoSequence();
    void testCmdPackToWorkProperties();

    void testNoInterfaceNotBusy();
    void testEmptyPackNotBusy();
    void testNotOpenInterfaceNotBusy();
    void testOpenInterfaceBusy();

    void testNoInterfaceNotification();
    void testNotOpenInterfaceNotifications();

    void testDisconnectBeforeEnqueue();
    void testDisconnectWhileWorking();
    void testDisconnectWhileWorkingMultipleNotifications();

    void testStopOnFirstErrorFullResponse();
    void testStopOnFirstErrorPartResponse();
    void testContinueOnErrorFullResponse();
    void testContinueOnErrorPartResponse();

//    void testCloseToSpamAccepted();
//    void testSpamRejected();

//    void testReplacePending();
//    void testReplacePendingSpam(); //???

//    void testOnePacketSingleIoOK();
//    void testTwoPacketSingleIoOK();

//    void testOnePacketMultipleIoOK();
//    void testTwoPacketMultipleIoOK();

private:
    QList<cWorkerCommandPacket> m_listWorkPacksReceived;
};

#endif // SOURCEIOWORKERENTRYTEST_H
