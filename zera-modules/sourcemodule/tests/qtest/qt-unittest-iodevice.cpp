#include "main-unittest-qt.h"
#include "qt-unittest-iodevice.h"
#include "io-device/iodevicedemo.h"
#include "io-device/iotransferdatasingle.h"

static QObject* pIoDeviceTest = addTest(new IoDeviceTest);

void IoDeviceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_listReceivedData.clear();
}

void IoDeviceTest::generateOutOfLimitsInterface()
{
    qputenv("QT_FATAL_CRITICALS", "0");
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_TYPE_COUNT);
    qputenv("QT_FATAL_CRITICALS", "1");
    QCOMPARE(interface, nullptr);
}

void IoDeviceTest::generateTypeSet()
{
    for(int type = 0; type<SERIAL_DEVICE_TYPE_COUNT; type++) {
        tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(IoDeviceTypes(type));
        QCOMPARE(IoDeviceTypes(type), interface->type());
    }
}

void IoDeviceTest::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_ioDataForSingleUse->m_dataReceived);
    if(error) {
        m_errorsReceived++;
    }
}

void IoDeviceTest::baseReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    checkIds(interface);
}

void IoDeviceTest::demoReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    checkIds(interface);
}

void IoDeviceTest::serialReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    checkIds(interface);
}

void IoDeviceTest::baseCannotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    interface->open("/dev/base/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoDeviceTest::demoCanOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QString strBefore = "/dev/demo/foo";
    interface->open(strBefore);
    QCOMPARE(interface->isOpen(), true);
    QCOMPARE(strBefore, interface->getDeviceInfo());
}

void IoDeviceTest::serialCannotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    interface->open("/dev/serial/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoDeviceTest::baseReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoDeviceTest::baseReportsErrorOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    interface->open(QString());
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoDeviceTest::demoReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoDeviceTest::demoReportsNoErrorOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open(QString());
    QVERIFY(interface->isOpen());
    checkNotifications(interface, 1, 0);
}

void IoDeviceTest::serialReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoDeviceTest::demoDelayNotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 1, 1);
}

void IoDeviceTest::demoDelayOpenDontWait()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 0, 0);
}

void IoDeviceTest::demoOpenDelayWait()
{
    tIoDeviceShPtr interface = createOpenDemoInterface(1);
    interface->sendAndReceive(m_ioDataForSingleUse);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoDeviceTest::demoResponseList()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "0";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10); // one I/O at a time

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "1";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "2";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoResponseListDelay()
{
    tIoDeviceShPtr interface = createOpenDemoInterface(10);

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "0";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "1";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "2";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], "0\r");
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoResponseListErrorInjection()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "0";
    m_ioDataForSingleUse->m_demoErrorResponse = true;
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30); // one I/O at a time

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "1";
    m_ioDataForSingleUse->m_demoErrorResponse = false;
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    m_ioDataForSingleUse->m_bytesExpectedLead = "2";
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(30);

    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], IoTransferDataSingle::demoErrorResponseData);
    QCOMPARE(m_listReceivedData[1], "1\r");
    QCOMPARE(m_listReceivedData[2], "2\r");
}

void IoDeviceTest::demoDelayFollowsDelay()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());

    interface->setReadTimeoutNextIo(5000); // must be ignored
    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    demoInterface->setResponseDelay(false, 5000);
    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);

    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDeviceTest::demoDelayFollowsTimeout()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());

    interface->setReadTimeoutNextIo(0);    // default is not 0
    demoInterface->setResponseDelay(true, 5000 /* must be ignored */);
    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    interface->setReadTimeoutNextIo(5000);
    m_ioDataForSingleUse = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    interface->sendAndReceive(m_ioDataForSingleUse);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoDeviceTest::baseCannotClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BROKEN);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IoDeviceBrokenDummy::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void IoDeviceTest::demoCanClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IoDeviceBrokenDummy::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void IoDeviceTest::checkIds(tIoDeviceShPtr interface)
{
    tIoTransferDataSingleShPtr dummyIoData = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    int ioID = interface->sendAndReceive(dummyIoData);
    QCOMPARE(ioID, 0);
    ioID = interface->sendAndReceive(dummyIoData);
    QCOMPARE(ioID, 1);
}

void IoDeviceTest::checkNotifications(tIoDeviceShPtr interface, int total, int errors)
{
    connect(interface.get(), &IoDeviceBrokenDummy::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    tIoTransferDataSingleShPtr dummyIoData = tIoTransferDataSingleShPtr(new IoTransferDataSingle);
    interface->sendAndReceive(dummyIoData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

tIoDeviceShPtr IoDeviceTest::createOpenDemoInterface(int responseDelay)
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    connect(interface.get(), &IoDeviceBrokenDummy::sigIoFinished, this, &IoDeviceTest::onIoFinish);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->open(QString());
    if(responseDelay) {
        demoInterface->setResponseDelay(false, responseDelay);
    }
    return interface;
}
