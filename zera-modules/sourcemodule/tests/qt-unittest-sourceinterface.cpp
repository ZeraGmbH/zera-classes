#include "main-unittest-qt.h"
#include "qt-unittest-sourceinterface.h"

static QObject* pSourceInterfaceTest = addTest(new SourceInterfaceTest);

void SourceInterfaceTest::init()
{
    m_ioIDReceived = -1;
    m_ioFinishReceived = 0;
}

void SourceInterfaceTest::onIoFinish(int ioID)
{
    m_ioIDReceived = ioID;
    m_ioFinishReceived++;
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
    QCOMPARE(m_ioFinishReceived, 1);
}

void SourceInterfaceTest::testDemoFinishQueued()
{
    tSourceInterfaceShPtr interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    disconnect(interface.get(), &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QCOMPARE(m_ioFinishReceived, 0);
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

