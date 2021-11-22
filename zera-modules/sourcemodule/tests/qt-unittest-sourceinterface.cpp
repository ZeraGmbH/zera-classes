#include "main-unittest-qt.h"
#include "qt-unittest-sourceinterface.h"

static QObject* pSourceInterfaceTest = addTest(new SourceInterfaceTest);

void SourceInterfaceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceiveCount = 0;
    m_receivedData.clear();
    m_listReceivedData.clear();
}

void SourceInterfaceTest::onIoFinish(int ioID)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceiveCount++;
    m_listReceivedData.append(m_receivedData);
}

void SourceInterfaceTest::testIoIDNotSetForBaseInterface()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    QByteArray dummyArray;
    int ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 0);
}

void SourceInterfaceTest::testIoIDSetForDemoInterface()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    QVERIFY(demoInterface->open(QString()));
    QByteArray dummyArray;
    int ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 1);
    ioID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(ioID, 2);
}

void SourceInterfaceTest::testDemoFinish()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    bool connected = connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QCOMPARE(connected, true);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(1000);
    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QCOMPARE(m_ioFinishReceiveCount, 1);
}

void SourceInterfaceTest::testDemoFinishQueued()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QCOMPARE(m_ioFinishReceiveCount, 0);
}

void SourceInterfaceTest::testDemoFinishIDs()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);

    QByteArray dummyReceive;
    int startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_ioIDReceived, startId);
    startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_ioIDReceived, startId);

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::testInterfaceTypesSetProperly()
{
    for(int type = 0; type<SOURCE_INTERFACE_TYPE_COUNT; type++) {
        tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes(type));
        QCOMPARE(SourceInterfaceTypes(type), interface->type());
    }
}

void SourceInterfaceTest::testDemoDelayNotOpen()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->setResponseDelay(5000);

    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_ioFinishReceiveCount, 1);

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::testDemoDelayDontWait()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(5000);

    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_ioFinishReceiveCount, 0);

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::testDemoDelayWait()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    demoInterface->setResponseDelay(100);

    QByteArray dummyReceive;
    int startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(300);
    QCOMPARE(m_ioFinishReceiveCount, 1);
    QCOMPARE(m_ioIDReceived, startId);

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::testDemoResponseList()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
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

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

void SourceInterfaceTest::testDemoResponseListDelay()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(interface.get());
    demoInterface->open(QString());
    QList<QByteArray> testResponses = QList<QByteArray>() << "0\r" << "1\r" << "2\r";
    demoInterface->setResponses(testResponses);
    demoInterface->setResponseDelay(50);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(200); // one I/O at a time
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(200);
    interface->sendAndReceive(QByteArray(), &m_receivedData);
    QTest::qWait(200);
    QCOMPARE(m_ioFinishReceiveCount, 3);
    QCOMPARE(m_listReceivedData, testResponses);

    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
}

