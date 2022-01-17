#include "main-unittest-qt.h"
#include "qt-unittest-iointerface.h"
#include "io-interface/iointerfacedemo.h"

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
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_TYPE_COUNT);
    QCOMPARE(interface, nullptr);
}

void IoInterfaceTest::generateTypeSet()
{
    for(int type = 0; type<SOURCE_INTERFACE_TYPE_COUNT; type++) {
        tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(IoInterfaceTypes(type));
        QCOMPARE(IoInterfaceTypes(type), interface->type());
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
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    checkIds(interface);
}

void IoInterfaceTest::demoReturnsIds()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    checkIds(interface);
}

void IoInterfaceTest::serialReturnsIds()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    checkIds(interface);
}

void IoInterfaceTest::baseCannotOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    interface->open("/dev/base/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoInterfaceTest::demoCanOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    QString strBefore = "/dev/demo/foo";
    interface->open(strBefore);
    QCOMPARE(interface->isOpen(), true);
    QCOMPARE(strBefore, interface->getDeviceInfo());
}

void IoInterfaceTest::serialCannotOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    interface->open("/dev/serial/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void IoInterfaceTest::baseReportsErrorClose()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::baseReportsErrorOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    interface->open(QString());
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoReportsErrorClose()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoReportsNoErrorOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open(QString());
    QVERIFY(interface->isOpen());
    checkNotifications(interface, 1, 0);
}

void IoInterfaceTest::serialReportsErrorClose()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoDelayNotOpen()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 1, 1);
}

void IoInterfaceTest::demoDelayOpenDontWait()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 0, 0);
}

void IoInterfaceTest::demoOpenDelayWait()
{
    tIoInterfaceShPtr interface = createOpenDemoInterface(1);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void IoInterfaceTest::demoResponseList()
{
    tIoInterfaceShPtr interface = createOpenDemoInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
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

    disconnect(interface.get(), &IoInterfaceBase::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoResponseListDelay()
{
    tIoInterfaceShPtr interface = createOpenDemoInterface(10);
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
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

    disconnect(interface.get(), &IoInterfaceBase::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoResponseListErrorInjection()
{
    tIoInterfaceShPtr interface = createOpenDemoInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
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

    disconnect(interface.get(), &IoInterfaceBase::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
}

void IoInterfaceTest::demoDelayFollowsDelay()
{
    tIoInterfaceShPtr interface = createOpenDemoInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
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
    tIoInterfaceShPtr interface = createOpenDemoInterface();
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
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
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_BASE);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IoInterfaceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void IoInterfaceTest::demoCanClose()
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &IoInterfaceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void IoInterfaceTest::checkIds(tIoInterfaceShPtr interface)
{
    QByteArray dummyArray;
    int ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 0);
    ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 1);
}

void IoInterfaceTest::checkNotifications(tIoInterfaceShPtr interface, int total, int errors)
{
    connect(interface.get(), &IoInterfaceBase::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

tIoInterfaceShPtr IoInterfaceTest::createOpenDemoInterface(int responseDelay)
{
    tIoInterfaceShPtr interface = IoInterfaceFactory::createIoInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &IoInterfaceBase::sigIoFinished, this, &IoInterfaceTest::onIoFinish);
    IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    if(responseDelay) {
        demoInterface->setResponseDelay(false, responseDelay);
    }
    return interface;
}
