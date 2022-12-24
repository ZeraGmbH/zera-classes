#include "test_sourcescanner.h"
#include "test_globals.h"
#include "iodevicefactory.h"
#include "sourcescannerwithinstancecount-forunittest.h"
#include "source-scanner/sourcescanneriodemo.h"
#include "source-scanner/sourcescanneriozeraserial.h"

QTEST_MAIN(test_sourcescanner);

void test_sourcescanner::init()
{
    m_scanFinishCount = 0;
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
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    scanner->startScan();
    scanner = nullptr;

    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotDirectConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &test_sourcescanner::onScanFinishedForCheckInstanceCount,
            Qt::DirectConnection);
    scanner->startScan();
    scanner = nullptr;

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotQueuedConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &test_sourcescanner::onScanFinishedForCheckInstanceCount,
            Qt::QueuedConnection);
    scanner->startScan();
    scanner = nullptr;

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void test_sourcescanner::scannerSurvivesUntilSlotLambdaConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    int scanFinish = 0;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] {
        scanFinish++;
        QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    });
    scanner->startScan();
    scanner = nullptr;

    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(scanFinish, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}


////////////////////////////////////////////////////////////////////////
// !!! USE SourceScanner only on test below !!!
////////////////////////////////////////////////////////////////////////

void test_sourcescanner::uuidPassedIdentical()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);

    QUuid uuid = QUuid::createUuid();
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy, uuid);

    QUuid uuidReceived;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        uuidReceived = scanner->getUuid();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(uuid, uuidReceived);
}

void test_sourcescanner::ioDevicePassedIdentical()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoDeviceBase::Ptr ioDeviceReceived;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        ioDeviceReceived = scanner->getIoDevice();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(ioDevice.get(), ioDeviceReceived.get());
}

void test_sourcescanner::scannerReportsInvalidSourceAfterCreation()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    QCOMPARE(scanner->getSourcePropertiesFound().wasSet(), false);
}

void test_sourcescanner::scannerReportsValidSourceAfterDemoIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props;
    QCOMPARE(props.wasSet(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), true);
}

void test_sourcescanner::scannerReportsValidSourceAfterZeraIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoZeraSerial);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props;
    QCOMPARE(props.wasSet(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), true);
}

void test_sourcescanner::scannerReportsInvalidSourceAfterBrokenIo()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL);
    QCOMPARE(props.wasSet(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}

void test_sourcescanner::scannerReportsInvalidSourceAfterDemoIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoQueueEntryList list = ioStrategy->getIoQueueEntriesForScan();
    for(IoQueueEntry::Ptr entry : qAsConst(list)) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx) {
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
        }
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL);
    QCOMPARE(props.wasSet(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}

void test_sourcescanner::scannerReportsInvalidSourceAfterZeraIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoZeraSerial);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoQueueEntryList list = ioStrategy->getIoQueueEntriesForScan();
    for(IoQueueEntry::Ptr entry : qAsConst(list)) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx) {
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
        }
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "", SourceProtocols::ZERA_SERIAL);
    QCOMPARE(props.wasSet(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(shortQtEventTimeout);
    QCOMPARE(props.wasSet(), false);
}
