#include "test_iodevice.h"
#include "test_globals.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"

QTEST_MAIN(IoDevice)

void IoDevice::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "");
    m_listReceivedData.clear();
}

void IoDevice::generateBrokenIoDeviceForOutOfLimitType()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes(-1));
    QCOMPARE(IoDeviceTypes::BROKEN, ioDevice->getType());
}

void IoDevice::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_ioDataForSingleUse->getBytesReceived());
    if(error) {
        m_errorsReceived++;
    }
}

void IoDevice::baseReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    checkIds(ioDevice);
}

void IoDevice::demoReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    checkIds(ioDevice);
}

void IoDevice::serialReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    checkIds(ioDevice);
}

void IoDevice::baseCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open("/dev/base/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDevice::demoCanOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QString strBefore = "/dev/demo/foo";
    ioDevice->open(strBefore);
    QCOMPARE(ioDevice->isOpen(), true);
    QCOMPARE(strBefore, ioDevice->getDeviceInfo());
}

void IoDevice::serialCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    ioDevice->open("/dev/serial/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDevice::baseReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDevice::baseReportsErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDevice::demoReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDevice::demoReportsNoErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(QString());
    QVERIFY(ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 0);
}

void IoDevice::serialReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDevice::demoDelayNotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 1, 1);
}

void IoDevice::demoDelayOpenDontWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 0, 0);
}

void IoDevice::demoOpenDelayWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected(1);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoDevice::demoResponseList()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDevice::demoResponseListDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected(10);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDevice::demoResponseListErrorInjection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    m_ioDataForSingleUse->getDemoResponder()->activateErrorResponse();
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 2);
    QCOMPARE(m_listReceivedData[0], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[1], "1\r");
}

void IoDevice::demoResponseAlwaysError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIO = static_cast<IoDeviceDemo*>(ioDevice.get());

    demoIO->setAllTransfersError(true);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    demoIO->setAllTransfersError(false);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[1], IoTransferDemoResponder::getDefaultErrorResponse());
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDevice::demoDelayFollowsDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    // huge timeout must be ignored
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    demoIoDevice->setResponseDelay(false, 5000);
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);

    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDevice::demoDelayFollowsTimeout()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 0);
    demoIoDevice->setResponseDelay(true, 5000 /* must be ignored */);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDevice::baseCannotClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open("");

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->close();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void IoDevice::demoCanClose()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->close();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void IoDevice::checkIds(IoDeviceBase::Ptr ioDevice)
{
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    int ioID1 = ioDevice->sendAndReceive(dummyIoData);
    int ioID2 = ioDevice->sendAndReceive(dummyIoData);
    QVERIFY(ioID1 != ioID2);
}

void IoDevice::checkNotifications(IoDeviceBase::Ptr ioDevice, int total, int errors)
{
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDevice::onIoFinish);
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

IoDeviceBase::Ptr IoDevice::createOpenDemoIoDeviceWithDelayAndConnected(int responseDelay)
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDevice::onIoFinish);
    if(responseDelay) {
        IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
        demoIoDevice->setResponseDelay(false, responseDelay);
    }
    return ioDevice;
}
