#include "main-unittest-qt.h"
#include "qt-unittest-sourceinterface.h"

static QObject* pSourceInterfaceTest = addTest(new SourceInterfaceTest);

void SourceInterfaceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_errorsReceived = 0;
    m_receivedData.clear();
    m_listReceivedData.clear();
}

void SourceInterfaceTest::generateOOLInterface()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_TYPE_COUNT);
    QCOMPARE(interface, nullptr);
}

void SourceInterfaceTest::generateTypeSet()
{
    for(int type = 0; type<SOURCE_INTERFACE_TYPE_COUNT; type++) {
        tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes(type));
        QCOMPARE(SourceInterfaceTypes(type), interface->type());
    }
}

void SourceInterfaceTest::onIoFinish(int ioID, bool error)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_receivedData);
    if(error) {
        m_errorsReceived++;
    }
}

void SourceInterfaceTest::baseReturnsIds()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    checkIds(interface);
}

void SourceInterfaceTest::demoReturnsIds()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    checkIds(interface);
}

void SourceInterfaceTest::serialReturnsIds()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    checkIds(interface);
}

void SourceInterfaceTest::baseCannotOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    interface->open("/dev/base/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void SourceInterfaceTest::demoCanOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QString strBefore = "/dev/demo/foo";
    interface->open(strBefore);
    QCOMPARE(interface->isOpen(), true);
    QCOMPARE(strBefore, interface->getDeviceInfo());
}

void SourceInterfaceTest::serialCannotOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    interface->open("/dev/serial/foo");
    QCOMPARE(interface->isOpen(), false);
    QVERIFY(interface->getDeviceInfo().isEmpty());
}

void SourceInterfaceTest::baseReportsErrorClose()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void SourceInterfaceTest::baseReportsErrorOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    interface->open(QString());
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void SourceInterfaceTest::demoReportsErrorClose()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void SourceInterfaceTest::demoReportsNoErrorOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    interface->open(QString());
    QVERIFY(interface->isOpen());
    checkNotifications(interface, 1, 0);
}

void SourceInterfaceTest::serialReportsErrorClose()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_ASYNCSERIAL);
    QVERIFY(!interface->isOpen());
    checkNotifications(interface, 1, 1);
}

void SourceInterfaceTest::demoDelayNotOpen()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 1, 1);
}

void SourceInterfaceTest::demoDelayOpenDontWait()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(false, 5000);
    checkNotifications(interface, 0, 0);
}

void SourceInterfaceTest::demoOpenDelayWait()
{
    tSourceInterfaceShPtr interface = createOpenDemoInterface(1);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(100);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_errorsReceived, 0);
}

void SourceInterfaceTest::demoResponseList()
{
    tSourceInterfaceShPtr interface = createOpenDemoInterface();
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->setResponses(testResponses);

    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData, testResponses);

    disconnect(interface.get(), &SourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::demoResponseListDelay()
{
    tSourceInterfaceShPtr interface = createOpenDemoInterface(10);
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->setResponses(testResponses);

    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(50);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData, testResponses);

    disconnect(interface.get(), &SourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::demoDelayFollowsDelay()
{
    tSourceInterfaceShPtr interface = createOpenDemoInterface();
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    interface->setReadTimeoutNextIo(5000); // must be ignored
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    demoInterface->setResponseDelay(false, 5000);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void SourceInterfaceTest::demoDelayFollowsTimeout()
{
    tSourceInterfaceShPtr interface = createOpenDemoInterface();
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
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

void SourceInterfaceTest::baseCannotClose()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &SourceInterfaceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 0);
}

void SourceInterfaceTest::demoCanClose()
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    interface->open("");

    int countDiconnectReceived = 0;
    connect(interface.get(), &SourceInterfaceBase::sigDisconnected, [&] {
        countDiconnectReceived++;
    });
    interface->simulateExternalDisconnect();
    QTest::qWait(1);
    QCOMPARE(countDiconnectReceived, 1);
}

void SourceInterfaceTest::checkIds(tSourceInterfaceShPtr interface)
{
    QByteArray dummyArray;
    int ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 0);
    ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 1);
}

void SourceInterfaceTest::checkNotifications(tSourceInterfaceShPtr interface, int total, int errors)
{
    connect(interface.get(), &SourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QCOMPARE(m_ioFinishReceiveCount, 0); // check for queued
    QCOMPARE(m_errorsReceived, 0);
    QTest::qWait(10);
    QCOMPARE(m_ioFinishReceiveCount, total);
    QCOMPARE(m_errorsReceived, errors);
}

tSourceInterfaceShPtr SourceInterfaceTest::createOpenDemoInterface(int responseDelay)
{
    tSourceInterfaceShPtr interface = SourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &SourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    if(responseDelay) {
        demoInterface->setResponseDelay(false, responseDelay);
    }
    return interface;
}
