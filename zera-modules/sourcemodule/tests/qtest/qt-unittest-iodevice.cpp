#include "main-unittest-qt.h"
#include "qt-unittest-iodevice.h"
#include "io-device/iodevicefactory.h"
#include "io-device/iodevicedemo.h"
#include "io-device/iotransferdatasinglefactory.h"

static QObject* pIoDeviceTest = addTest(new IoDeviceTest);

void IoDeviceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData();
    m_listReceivedData.clear();
}

void IoDeviceTest::generateOutOfLimitsInterface()
{
    qputenv("QT_FATAL_CRITICALS", "0");
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_TYPE_COUNT);
    qputenv("QT_FATAL_CRITICALS", "1");
    QCOMPARE(ioDevice, nullptr);
}

void IoDeviceTest::generateTypeSet()
{
    for(int type = 0; type<SERIAL_DEVICE_TYPE_COUNT; type++) {
        IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes(type));
        QCOMPARE(IoDeviceTypes(type), ioDevice->type());
    }
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
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    checkIds(ioDevice);
}

void IoDeviceTest::demoReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    checkIds(ioDevice);
}

void IoDeviceTest::serialReturnsIds()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    checkIds(ioDevice);
}

void IoDeviceTest::baseCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    ioDevice->open("/dev/base/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDeviceTest::demoCanOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QString strBefore = "/dev/demo/foo";
    ioDevice->open(strBefore);
    QCOMPARE(ioDevice->isOpen(), true);
    QCOMPARE(strBefore, ioDevice->getDeviceInfo());
}

void IoDeviceTest::serialCannotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    ioDevice->open("/dev/serial/foo");
    QCOMPARE(ioDevice->isOpen(), false);
    QVERIFY(ioDevice->getDeviceInfo().isEmpty());
}

void IoDeviceTest::baseReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::baseReportsErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    ioDevice->open(QString());
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoReportsNoErrorOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    ioDevice->open(QString());
    QVERIFY(ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 0);
}

void IoDeviceTest::serialReportsErrorClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    QVERIFY(!ioDevice->isOpen());
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoDelayNotOpen()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 1, 1);
}

void IoDeviceTest::demoDelayOpenDontWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
    demoIoDevice->setResponseDelay(false, 5000);
    checkNotifications(ioDevice, 0, 0);
}

void IoDeviceTest::demoOpenDelayWait()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected(1);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoDeviceTest::demoResponseList()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoResponseListDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected(10);

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "0");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "1");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoResponseListErrorInjection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "0");
    m_ioDataForSingleUse->m_demoErrorResponse = true;
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "1");
    m_ioDataForSingleUse->m_demoErrorResponse = false;
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "2");
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], IoTransferDataSingle::demoErrorResponseData);
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoDelayFollowsDelay()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    // huge timeout must be ignored
    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    demoIoDevice->setResponseDelay(false, 5000);
    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData();
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDeviceTest::demoDelayFollowsTimeout()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();
    IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "\r", "", 0);
    demoIoDevice->setResponseDelay(true, 5000 /* must be ignored */);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    m_ioDataForSingleUse = IoTransferDataSingleFactory::createIoData("", "\r", "", 5000);
    ioDevice->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDeviceTest::baseCannotClose()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    ioDevice->open("");

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void IoDeviceTest::demoCanClose()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterfaceWithDelayAndConnected();

    int countDiconnectReceived = 0;
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    ioDevice->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void IoDeviceTest::checkIds(IoDeviceBase::Ptr ioDevice)
{
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingleFactory::createIoData();
    int ioID = ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(ioID, 0);
    ioID = ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(ioID, 1);
}

void IoDeviceTest::checkNotifications(IoDeviceBase::Ptr ioDevice, int total, int errors)
{
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    IoTransferDataSingle::Ptr dummyIoData = IoTransferDataSingleFactory::createIoData();
    ioDevice->sendAndReceive(dummyIoData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

IoDeviceBase::Ptr IoDeviceTest::createOpenDemoInterfaceWithDelayAndConnected(int responseDelay)
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoInterface();
    connect(ioDevice.get(), &IoDeviceBase::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    if(responseDelay) {
        IoDeviceDemo* demoIoDevice = static_cast<IoDeviceDemo*>(ioDevice.get());
        demoIoDevice->setResponseDelay(false, responseDelay);
    }
    return ioDevice;
}
