#ifndef SOURCEIOWORKERENTRYTEST_H
#define SOURCEIOWORKERENTRYTEST_H

#include <QObject>
#include "sourceioworker.h"

class SourceIoWorkerTest : public QObject
{
    Q_OBJECT
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

//    void testNoInterfaceMultipleNotifications();
//    void testNotOpenInterfaceMultipleNotifications();

    void testDisconnectBeforeEnqueue();
//    void testDisconnectWhileWorking();
//    void testDisconnectWhileWorkingMultipleNotifications();

//    void testStopOnFirstSuccess();
//    void testStopOnFirstError();
//    void testContinueOnError();

//    void testCloseToSpamAccepted();
//    void testSpamRejected();

//    void testReplacePending();
//    void testReplacePendingSpam(); //???

//    void testOnePacketSingleIoOK();
//    void testTwoPacketSingleIoOK();

//    void testOnePacketMultipleIoOK();
//    void testTwoPacketMultipleIoOK();
};

#endif // SOURCEIOWORKERENTRYTEST_H
