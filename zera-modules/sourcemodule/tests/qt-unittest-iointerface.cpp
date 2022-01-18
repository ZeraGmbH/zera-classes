#include "main-unittest-qt.h"
#include "qt-unittest-iointerface.h"
#include "io-device/iodevicedemo.h"

static QObject* pIoInterfaceTest = addTest(new IoInterfaceTest);

void IoInterfaceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_receivedData.clear();
    m_listReceivedData.clear();
}

void IoInterfaceTest::generateOOLInterface()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_TYPE_COUNT);
    QCOMPARE(interface, nullptr);
}

void IoInterfaceTest::generateTypeSet()
{
    for(int type = 0; type<SERIAL_DEVICE_TYPE_COUNT; type++) {
        tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(IoDeviceTypes(type));
        QCOMPARE(IoDeviceTypes(type), interface->type());
    }
}

void IoInterfaceTest::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_receivedData);
    if(error) {
        m_errorsReceived++;
    }
}

void IoInterfaceTest::baseReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BASE);
    checkIds(interface);
}

void IoInterfaceTest::demoReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    checkIds(interface);
}

void IoInterfaceTest::serialReturnsIds()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    checkIds(interface);
}

void IoInterfaceTest::baseCannotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BASE);
    interface->open("/dev/base/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoInterfaceTest::demoCanOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QString strBefore = "/dev/demo/foo";
    interface->open(strBefore);
    QCOMPARE(interface->isOpen(), true);
    QCOMPARE(strBefore, interface->getDeviceInfo());
}

void IoInterfaceTest::serialCannotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    interface->open("/dev/serial/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoInterfaceTest::baseReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BASE);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::baseReportsErrorOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BASE);
    interface->open(QString());
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoReportsNoErrorOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open(QString());
    QVERIFY(interface->isOpen());
    checkNotifications(interface, 1, 0);
}

void IoInterfaceTest::serialReportsErrorClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_ASYNCSERIAL);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoDelayNotOpen()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoDelayOpenDontWait()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 0, 0);
}

void IoInterfaceTest::demoOpenDelayWait()
{
    tIoDeviceShPtr interface = createOpenDemoInterface(1);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoInterfaceTest::demoResponseList()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->appendResponses(testResponses);

    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData, testResponses);

    disconnect(interface.get(), &IODeviceBaseSerial::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoResponseListDelay()
{
    tIoDeviceShPtr interface = createOpenDemoInterface(10);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->appendResponses(testResponses);

    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData, testResponses);

    disconnect(interface.get(), &IODeviceBaseSerial::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoResponseListErrorInjection()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->appendResponses(testResponses);

    QList<QByteArray>& injectError = demoInterface->getResponsesForErrorInjection();
    QByteArray errInject = "abc";
    injectError[0] = errInject;

    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData[0], errInject);
    QCOMPARE(m_listReceivedData[1], testResponses[1]);
    QCOMPARE(m_listReceivedData[2], testResponses[2]);

    disconnect(interface.get(), &IODeviceBaseSerial::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoDelayFollowsDelay()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    interface->setReadTimeoutNextIo(5000); // must be ignored
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    demoInterface->setResponseDelay(false, 5000);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoInterfaceTest::demoDelayFollowsTimeout()
{
    tIoDeviceShPtr interface = createOpenDemoInterface();
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    interface->setReadTimeoutNextIo(0);    // default is not 0
    demoInterface->setResponseDelay(true, 5000 /* must be ignored */);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    interface->setReadTimeoutNextIo(5000);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void IoInterfaceTest::baseCannotClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_BASE);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IODeviceBaseSerial::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void IoInterfaceTest::demoCanClose()
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IODeviceBaseSerial::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void IoInterfaceTest::checkIds(tIoDeviceShPtr interface)
{
    QByteArray dummyArray;
    int ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 0);
    ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 1);
}

void IoInterfaceTest::checkNotifications(tIoDeviceShPtr interface, int total, int errors)
{
    connect(interface.get(), &IODeviceBaseSerial::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

tIoDeviceShPtr IoInterfaceTest::createOpenDemoInterface(int responseDelay)
{
    tIoDeviceShPtr interface = IoDeviceFactory::createIoDevice(SERIAL_DEVICE_DEMO);
    connect(interface.get(), &IODeviceBaseSerial::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
    IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(interface.get());
    demoInterface->open(QString());
    if(responseDelay) {
        demoInterface->setResponseDelay(false, responseDelay);
    }
    return interface;
}
