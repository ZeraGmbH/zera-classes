#include "test_iodevice.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "iodevicedemo.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>

QTEST_MAIN(test_iodevice)

void test_iodevice::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "");
    m_listReceivedData.clear();
    TimerFactoryQtForTest::enableTest();
}

void test_iodevice::generateBrokenIoDeviceForOutOfLimitType()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes(-1));
    QCOMPARE(IoDeviceTypes::BROKEN, ioDevice->getType());
}

void test_iodevice::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_ioDataForSingleUse->getBytesReceived());
    if(error) {
        m_errorsReceived++;
    }
}

void test_iodevice::baseReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    checkIds(ioDevice);
}

void test_iodevice::demoReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    checkIds(ioDevice);
}

void test_iodevice::serialReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    checkIds(ioDevice);
}

void test_iodevice::baseCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open("/dev/base/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void test_iodevice::demoCanOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QString strBefore = "/dev/demo/foo";
    ioDevice->open(strBefore);
    QCOMPARE(ioDevice->isOpen(), true);
    QCOMPARE(strBefore, ioDevice->getDeviceInfo());
}

void test_iodevice::serialCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    ioDevice->open("/dev/serial/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void test_iodevice::baseReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void test_iodevice::baseReportsErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void test_iodevice::demoReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void test_iodevice::demoReportsNoErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(QString());
    QVERIFY(ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 0);
}

void test_iodevice::serialReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void test_iodevice::demoDelayNotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 1, 1);
}

void test_iodevice::demoDelayOpenDontWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 0, 0);
}

void test_iodevice::demoOpenDelayWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected(1);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    TimeMachineForTest::getInstance()->processTimers(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void test_iodevice::demoResponseList()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents(); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void test_iodevice::demoResponseListDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected(10);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void test_iodevice::demoResponseListErrorInjection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    m_ioDataForSingleUse->getDemoResponder()->activateErrorResponse();
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30);

    QCOMPARE(m_ioFinishReceiveCount, 2);
    QCOMPARE(m_listReceivedData[0], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[1], "1\r");
}

void test_iodevice::demoResponseAlwaysError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());

    demoIO->setAllTransfersError(true);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30);

    demoIO->setAllTransfersError(false);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    TimeMachineForTest::getInstance()->processTimers(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[1], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void test_iodevice::demoDelayFollowsDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    // huge timeout must be ignored
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();
    QCOMPARE(m_ioFinishReceiveCount, 1);

    demoIoDevice->setResponseDelay(false, 5000);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();

    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void test_iodevice::demoDelayFollowsTimeout()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 0);
    demoIoDevice->setResponseDelay(true, 5000 /* must be ignored */);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();
    QCOMPARE(m_ioFinishReceiveCount, 1);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCoreApplication::processEvents();
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void test_iodevice::baseCannotClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open("");

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->close();
    TimeMachineForTest::getInstance()->processTimers(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void test_iodevice::demoCanClose()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->close();
    TimeMachineForTest::getInstance()->processTimers(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void test_iodevice::checkIds(IoDeviceBase::Ptr ioDevice)
{
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    int ioID1 = ioDevice->sendAndReceive(dummyIoData);
    int ioID2 = ioDevice->sendAndReceive(dummyIoData);
    QVERIFY(ioID1 != ioID2);
}

void test_iodevice::checkNotifications(IoDeviceBase::Ptr ioDevice, int total, int errors)
{
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &test_iodevice::onIoFinish);
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QCoreApplication::processEvents();
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

IoDeviceBase::Ptr test_iodevice::createOpenDemoIoDeviceWithDelayAndConnected(int responseDelay)
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &test_iodevice::onIoFinish);
    if(responseDelay) {
        IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
        demoIoDevice->setResponseDelay(false, responseDelay);
    }
    return ioDevice;
}
