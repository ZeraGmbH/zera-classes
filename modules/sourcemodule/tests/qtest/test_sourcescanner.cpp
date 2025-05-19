#include "test_sourcescanner.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "sourcescannerwithinstancecount-forunittest.h"
#include "sourcescanneriodemo.h"
#include "sourcescanneriozeraserial.h"
#include "timerfactoryqtfortest.h"
#include "timemachinefortest.h"

QTEST_MAIN(test_sourcescanner)

void test_sourcescanner::init()
{
    m_scanFinishCount = 0;
    TimerFactoryQtForTest::enableTest();
}

void test_sourcescanner::onScanFinishedForCheckInstanceCount(SourceScanner::Ptr scanner)
{
    Q_UNUSED(scanner);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    m_scanFinishCount++;
}

void test_sourcescanner::scannerDiesOnNoConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    scanner->startScan();
    scanner = nullptr;

    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotDirectConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &test_sourcescanner::onScanFinishedForCheckInstanceCount,
            Qt::DirectConnection);
    scanner->startScan();
    scanner = nullptr;

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotQueuedConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &test_sourcescanner::onScanFinishedForCheckInstanceCount,
            Qt::QueuedConnection);
    scanner->startScan();
    scanner = nullptr;

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotLambdaConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    int scanFinish = 0;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] {
        scanFinish++;
        QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    });
    scanner->startScan();
    scanner = nullptr;

    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(scanFinish, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}


////////////////////////////////////////////////////////////////////////
// !!! USE SourceScanner only on test below !!!
////////////////////////////////////////////////////////////////////////

void test_sourcescanner::uuidPassedIdentical()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    QUuid uuid = QUuid::createUuid();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoDemo>(), uuid);

    QUuid uuidReceived;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        uuidReceived = scanner->getUuid();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(uuid, uuidReceived);
}

void test_sourcescanner::ioDevicePassedIdentical()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    IoDeviceBase::Ptr ioDeviceReceived;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        ioDeviceReceived = scanner->getIoDevice();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(ioDevice.get(), ioDeviceReceived.get());
}

void test_sourcescanner::scannerReportsInvalidSourceAfterCreation()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    QCOMPARE(scanner->getSourcePropertiesFound().wasSet(), false);
}

void test_sourcescanner::scannerReportsValidSourceAfterDemoIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    SourceProperties props;
    QCOMPARE(props.wasSet(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), true);
}

void test_sourcescanner::scannerReportsValidSourceAfterZeraIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoZeraSerial>());

    SourceProperties props;
    QCOMPARE(props.wasSet(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), true);
}

void test_sourcescanner::scannerReportsInvalidSourceAfterBrokenIo()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::make_unique<SourceScannerIoDemo>());

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL, SourceProperties::EXTERNAL);
    QCOMPARE(props.wasSet(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}

class SourceScannerIoDemoForTest : public SourceScannerIoDemo
{
public:
    IoQueueGroupListPtr getQueueGroupList()
    {
        return m_scanIoGroupList;
    }
};

void test_sourcescanner::scannerReportsInvalidSourceAfterDemoIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    std::unique_ptr<SourceScannerIoDemoForTest> ioStrategy = std::make_unique<SourceScannerIoDemoForTest>();

    IoQueueGroupListPtr queueGroupList = ioStrategy->getQueueGroupList();
    for(auto entry : queueGroupList) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx)
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL, SourceProperties::EXTERNAL);
    QCOMPARE(props.wasSet(), true);

    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::move(ioStrategy));
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}

class SourceScannerIoZeraSerialForTest : public SourceScannerIoZeraSerial
{
public:
    IoQueueGroupListPtr getQueueGroupList()
    {
        return m_scanIoGroupList;
    }
};

void test_sourcescanner::scannerReportsInvalidSourceAfterZeraIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    std::unique_ptr<SourceScannerIoZeraSerialForTest> ioStrategy = std::make_unique<SourceScannerIoZeraSerialForTest>();

    IoQueueGroupListPtr queueGroupList = ioStrategy->getQueueGroupList();
    for(auto entry : queueGroupList) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx)
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL, SourceProperties::EXTERNAL);
    QCOMPARE(props.wasSet(), true);

    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, std::move(ioStrategy));
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    TimeMachineForTest::getInstance()->processTimers(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}
