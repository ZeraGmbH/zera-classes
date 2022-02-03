#include "main-unittest-qt.h"
#include "qt-unittest-iodevice.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"

static QObject* pIoDeviceTest = addTest(new IoDeviceTest);

void IoDeviceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_ioDataForSingleUse = IoTransferDataSingle::Ptr::create("", "");
    m_listReceivedData.clear();
}

void IoDeviceTest::generateBrokenIoDeviceForOutOfLimitType()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes(-1));
    QCOMPARE(IoDeviceTypes::BROKEN, ioDevice->getType());
}

void IoDeviceTest::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_ioDataForSingleUse->getDataReceived());
    if(error) {
        m_errorsReceived++;
    }
}

void IoDeviceTest::baseReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    checkIds(ioDevice);
}

void IoDeviceTest::demoReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    checkIds(ioDevice);
}

void IoDeviceTest::serialReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    checkIds(ioDevice);
}

void IoDeviceTest::baseCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open("/dev/base/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDeviceTest::demoCanOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QString strBefore = "/dev/demo/foo";
    ioDevice->open(strBefore);
    QCOMPARE(ioDevice->isOpen(), true);
    QCOMPARE(strBefore, ioDevice->getDeviceInfo());
}

void IoDeviceTest::serialCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    ioDevice->open("/dev/serial/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDeviceTest::baseReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::baseReportsErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoReportsNoErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    ioDevice->open(QString());
    QVERIFY(ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 0);
}

void IoDeviceTest::serialReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::ASYNCSERIAL);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoDelayNotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::DEMO);
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoDelayOpenDontWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 0, 0);
}

void IoDeviceTest::demoOpenDelayWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDeviceWithDelayAndConnected(1);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoDeviceTest::demoResponseList()
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

void IoDeviceTest::demoResponseListDelay()
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

void IoDeviceTest::demoResponseListErrorInjection()
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

void IoDeviceTest::demoDelayFollowsDelay()
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

void IoDeviceTest::demoDelayFollowsTimeout()
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

void IoDeviceTest::baseCannotClose()
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

void IoDeviceTest::demoCanClose()
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

void IoDeviceTest::checkIds(IoDeviceBase::Ptr ioDevice)
{
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    int ioID1 = ioDevice->sendAndReceive(dummyIoData);
    int ioID2 = ioDevice->sendAndReceive(dummyIoData);
    QVERIFY(ioID1 != ioID2);
}

void IoDeviceTest::checkNotifications(IoDeviceBase::Ptr ioDevice, int total, int errors)
{
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingle::Ptr::create("", "");
    ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

IoDeviceBase::Ptr IoDeviceTest::createOpenDemoIoDeviceWithDelayAndConnected(int responseDelay)
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    if(responseDelay) {
        IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
        demoIoDevice->setResponseDelay(false, responseDelay);
    }
    return ioDevice;
}
