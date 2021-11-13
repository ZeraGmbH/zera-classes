#include "main-unittest-qt.h"
#include "qt-unittest-sourceinterface.h"

static QObject* pSourceInterfaceTest = addTest(new SourceInterfaceTest);

void SourceInterfaceTest::init()
{
    m_transactionIDReceived = -1;
    m_ioFinishReceived = 0;
}

void SourceInterfaceTest::onIoFinish(int transactionID)
{
    m_transactionIDReceived = transactionID;
    m_ioFinishReceived++;
}

void SourceInterfaceTest::testTransactionIDNotSetForBaseInterface()
{
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_BASE);
    QByteArray dummyArray;
    int transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    delete interface;
    QCOMPARE(transactionID, 0);
}

void SourceInterfaceTest::testTransactionIDSetForDemoInterface()
{
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    QByteArray dummyArray;
    int transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    QCOMPARE(transactionID, 1);
    transactionID = interface->sendAndReceive(QByteArray(), &dummyArray);
    delete interface;
    QCOMPARE(transactionID, 2);
}

void SourceInterfaceTest::testDemoInterfaceFinish()
{
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    bool connected = connect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QCOMPARE(connected, true);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(1000);
    disconnect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    delete interface;
    QCOMPARE(m_ioFinishReceived, 1);
}

void SourceInterfaceTest::testDemoInterfaceFinishQueued()
{
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    QByteArray dummyReceive;
    interface->sendAndReceive(QByteArray(), &dummyReceive);
    disconnect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    delete interface;
    QCOMPARE(m_ioFinishReceived, 0);
}

void SourceInterfaceTest::testDemoInterfaceFinishIDs()
{
    cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SOURCE_INTERFACE_DEMO);
    connect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);

    QByteArray dummyReceive;
    int startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_transactionIDReceived, startId);
    startId = interface->sendAndReceive(QByteArray(), &dummyReceive);
    QTest::qWait(200);
    QCOMPARE(m_transactionIDReceived, startId);

    disconnect(interface, &cSourceInterfaceBase::sigIoFinished, this, &SourceInterfaceTest::onIoFinish);
    delete interface;
}

void SourceInterfaceTest::testInterfaceTypesSetProperly()
{
    for(int type = 0; type<SOURCE_INTERFACE_TYPE_COUNT; type++) {
        cSourceInterfaceBase* interface = cSourceInterfaceFactory::createSourceInterface(SourceInterfaceType(type));
        QCOMPARE(SourceInterfaceType(type), interface->type());
        delete interface;
    }
}

