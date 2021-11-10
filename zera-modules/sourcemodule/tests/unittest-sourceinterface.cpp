#include <gtest/gtest.h>
#include "sourceinterface.h"
#include <QTimer>
#include <QCoreApplication>

static void testInterfaceEvents(
        cSourceInterfaceBase* interface,
        std::function<void()>& funcIoStart,
        std::function<void(int transactionId)>& funcIoFinished,
        std::function<void()>& funcTimeout)
{
    char const *p = "sourceinterface-test";
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

// yeah we test testInterfaceEvents first
TEST(TEST_IO_AND_TIMEOUT_TEST, START_CALLED) {
    bool startCalled = false;
    std::function<void()> funcIoStart = [&](){
        startCalled = true;
    };
    std::function<void(int)> funcIoFinished = [](int){};
    std::function<void()> funcTimeout = [](){};
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    testInterfaceEvents(interface, funcIoStart, funcIoFinished, funcTimeout);
    delete interface;
    EXPECT_EQ(startCalled, true);
}

TEST(TEST_IO_AND_TIMEOUT_TEST, TIMEOUT_CALLED) {
    std::function<void()> funcIoStart = [](){};
    std::function<void(int)> funcIoFinished = [](int){};
    bool timeout = false;
    std::function<void()> funcTimeout = [&](){
        timeout = true;
    };
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    testInterfaceEvents(interface, funcIoStart, funcIoFinished, funcTimeout);
    delete interface;
    EXPECT_EQ(timeout, true);
}

// Now interface basic tests
TEST(TEST_SOURCEINTERFACE, TRANSACTION_ID_BASE) {
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    QByteArray dummyArray;
    int transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    delete interface;
    EXPECT_EQ(transactionID, 0);
}

TEST(TEST_SOURCEINTERFACE, TRANSACTION_ID_DEMO) {
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QByteArray dummyArray;
    int transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    EXPECT_EQ(transactionID, 1);
    transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    delete interface;
    EXPECT_EQ(transactionID, 2);
}

TEST(TEST_SOURCEINTERFACE, IO_DEMO_FINISH) {
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QByteArray dummyReceive;
    bool finishCalled = false;
    std::function<void()> funcIoStart = [&](){
        interface->sendAndReceive(QByteArray(), &dummyReceive);
        EXPECT_EQ(finishCalled, false); // Ensure queued
    };
    std::function<void(int)> funcIoFinished = [&](int){
        finishCalled = true;
    };
    std::function<void()> funcTimeout = [](){};
    testInterfaceEvents(interface, funcIoStart, funcIoFinished, funcTimeout);
    delete interface;
    EXPECT_EQ(finishCalled, true);
}

TEST(TEST_SOURCEINTERFACE, IO_DEMO_FINISH_ID) {
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QByteArray dummyReceive;
    int startId = 0;
    std::function<void()> funcIoStart = [&](){
        startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    };
    int finishId = 0;
    std::function<void(int)> funcIoFinished = [&](int id){
        finishId = id;
    };
    std::function<void()> funcTimeout = [](){};
    testInterfaceEvents(interface, funcIoStart, funcIoFinished, funcTimeout);
    delete interface;
    EXPECT_EQ(startId, finishId);
}

TEST(TEST_SOURCEINTERFACE, IO_TYPES_SET_PROPERLY) {
    for(int type = 0; type<SOURCE_INTERFACE_TYPE_COUNT; type++) {
        cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SourceInterfaceType(type));
        EXPECT_EQ(SourceInterfaceType(type), interface->type());
        delete interface;
    }
}
