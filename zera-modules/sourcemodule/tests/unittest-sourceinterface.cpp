#include <gtest/gtest.h>
#include "sourceinterface.h"
#include <QTimer>
#include <QCoreApplication>

static void ioAndTimeoutTest(
        cSourceInterfaceBase* interface,
        std::function<void()>& funcIoStart,
        std::function<void(int transactionId)>& funcIoFinished,
        std::function<void()>& funcTimeout)
{
    char const *p = "abc";
    char const **dummyParam = { &p };
    int argc = 1;
    QCoreApplication loop(argc, const_cast<char**>(dummyParam));

    QTimer timerForEventLoopSetup;
    timerForEventLoopSetup.setSingleShot(true);
    QTimer timerForTimeout;
    timerForTimeout.setSingleShot(true);

    QObject::connect(interface, &cSourceInterfaceBase::ioFinished, [&](int id) {
        funcIoFinished(id);
        timerForTimeout.stop();
        loop.quit();
    });
    QObject::connect( &timerForTimeout, &QTimer::timeout, [&]() {
        funcTimeout();
        loop.quit();
    });
    QObject::connect( &timerForEventLoopSetup, &QTimer::timeout, [&]() {
        // We have a running event loop here start our test
        timerForTimeout.start(300);
        funcIoStart();
    });
    timerForEventLoopSetup.start(0);
    loop.exec();
}

// yeah we test ioAndTimeoutTest first
TEST(TEST_IO_AND_TIMEOUT_TEST, START_CALLED) {
    bool startCalled = false;
    std::function<void()> funcIoStart = [&](){
        startCalled = true;
    };
    std::function<void(int)> funcIoFinished = [](int){};
    std::function<void()> funcTimeout = [](){};
    cSourceInterfaceBase interface;
    ioAndTimeoutTest(&interface, funcIoStart, funcIoFinished, funcTimeout);
    EXPECT_EQ(startCalled, true);
}

TEST(TEST_IO_AND_TIMEOUT_TEST, TIMEOUT_CALLED) {
    std::function<void()> funcIoStart = [](){};
    std::function<void(int)> funcIoFinished = [](int){};
    bool timeout = false;
    std::function<void()> funcTimeout = [&](){
        timeout = true;
    };
    cSourceInterfaceBase interface;
    ioAndTimeoutTest(&interface, funcIoStart, funcIoFinished, funcTimeout);
    EXPECT_EQ(timeout, true);
}

// Now interface basic tests
TEST(TEST_SOURCEINTERFACE, TRANSACTION_ID_BASE) {
    cSourceInterfaceBase interface;
    QByteArray dummyArray;
    int transactionID = interface.sendAndReceive(QByteArray(), &dummyArray);
    EXPECT_EQ(transactionID, 0);
}

TEST(TEST_SOURCEINTERFACE, TRANSACTION_ID_DEMO) {
    cSourceInterfaceDemo interface;
    QByteArray dummyArray;
    int transactionID = interface.sendAndReceive(QByteArray(), &dummyArray);
    EXPECT_EQ(transactionID, 1);
    transactionID = interface.sendAndReceive(QByteArray(), &dummyArray);
    EXPECT_EQ(transactionID, 2);
}

TEST(TEST_SOURCEINTERFACE, IO_DEMO_FINISH) {
    cSourceInterfaceDemo interface;
    QByteArray dummyReceive;
    bool finishCalled = false;
    std::function<void()> funcIoStart = [&](){
        interface.sendAndReceive(QByteArray(), &dummyReceive);
        EXPECT_EQ(finishCalled, false); // Ensure queued
    };
    std::function<void(int)> funcIoFinished = [&](int){
        finishCalled = true;
    };
    std::function<void()> funcTimeout = [](){};
    ioAndTimeoutTest(&interface, funcIoStart, funcIoFinished, funcTimeout);
    EXPECT_EQ(finishCalled, true);
}

TEST(TEST_SOURCEINTERFACE, IO_DEMO_FINISH_ID) {
    cSourceInterfaceDemo interface;
    QByteArray dummyReceive;
    int startId = 0;
    std::function<void()> funcIoStart = [&](){
        startId = interface.sendAndReceive(QByteArray(), &dummyReceive);
    };
    int finishId = 0;
    std::function<void(int)> funcIoFinished = [&](int id){
        finishId = id;
    };
    std::function<void()> funcTimeout = [](){};
    ioAndTimeoutTest(&interface, funcIoStart, funcIoFinished, funcTimeout);
    EXPECT_EQ(startId, finishId);
}
