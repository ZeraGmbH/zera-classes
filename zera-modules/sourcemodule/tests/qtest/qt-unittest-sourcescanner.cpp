#include "main-unittest-qt.h"
#include "qt-unittest-sourcescanner.h"
#include "io-device/iodevicefactory.h"
#include "sourcescannerwithinstancecount-forunittest.h"
#include "source-scanner/sourcescanneriodemo.h"
#include "source-scanner/sourcescanneriozeraserial.h"

static QObject* pSourceIoTest = addTest(new SourceScannerTest);

void SourceScannerTest::init()
{
    m_scanFinishCount = 0;
}

void SourceScannerTest::onScanFinishedForCheckInstanceCount(SourceScanner::Ptr scanner)
{
    Q_UNUSED(scanner);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    m_scanFinishCount++;
}

void SourceScannerTest::scannerDiesOnNoConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    scanner->startScan();
    scanner = nullptr;

    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 1);
    QTest::qWait(10);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void SourceScannerTest::scannerSurvivesUntilSlotDirectConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &SourceScannerTest::onScanFinishedForCheckInstanceCount,
            Qt::DirectConnection);
    scanner->startScan();
    scanner = nullptr;

    QTest::qWait(10);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void SourceScannerTest::scannerSurvivesUntilSlotQueuedConnection()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScannerWithInstanceCount::create(ioDevice, ioStrategy);

    connect(scanner.get(), &SourceScanner::sigScanFinished,
            this, &SourceScannerTest::onScanFinishedForCheckInstanceCount,
            Qt::QueuedConnection);
    scanner->startScan();
    scanner = nullptr;

    QTest::qWait(10);
    QCOMPARE(m_scanFinishCount, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}

void SourceScannerTest::scannerSurvivesUntilSlotLambdaConnection()
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

    QTest::qWait(10);
    QCOMPARE(scanFinish, 1);
    QCOMPARE(SourceScannerWithInstanceCount::getInstanceCount(), 0);
}


////////////////////////////////////////////////////////////////////////
// !!! USE SourceScanner only on test below !!!
////////////////////////////////////////////////////////////////////////

void SourceScannerTest::uuidPassedIdentical()
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
    QTest::qWait(10);
    QCOMPARE(uuid, uuidReceived);
}

void SourceScannerTest::ioDevicePassedIdentical()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoDeviceBase::Ptr ioDeviceReceived;
    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        ioDeviceReceived = scanner->getIoDevice();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(ioDevice.get(), ioDeviceReceived.get());
}

void SourceScannerTest::scannerReportsInvalidSourceAfterCreation()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    QCOMPARE(scanner->getSourcePropertiesFound().isValid(), false);
}

void SourceScannerTest::scannerReportsValidSourceAfterDemoIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props;
    QCOMPARE(props.isValid(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(props.isValid(), true);
}

void SourceScannerTest::scannerReportsValidSourceAfterZeraIo()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoZeraSerial);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props;
    QCOMPARE(props.isValid(), false);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(props.isValid(), true);
}

void SourceScannerTest::scannerReportsInvalidSourceAfterBrokenIo()
{
    IoDeviceBase::Ptr ioDevice = IoDeviceFactory::createIoDevice(IoDeviceTypes::BROKEN);
    ioDevice->open(QString());
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    SourceProperties props(SOURCE_MT_COMMON, "", "");
    QCOMPARE(props.isValid(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(props.isValid(), false);
}

void SourceScannerTest::scannerReportsInvalidSourceAfterDemoIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoDemo);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoQueueEntryList list = ioStrategy->getIoQueueEntriesForScan();
    for(IoQueueEntry::Ptr entry : list) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx) {
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
        }
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "");
    QCOMPARE(props.isValid(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(props.isValid(), false);
}

void SourceScannerTest::scannerReportsInvalidSourceAfterZeraIoResponseError()
{
    IoDeviceBase::Ptr ioDevice = createOpenDemoIoDevice();
    ISourceScannerStrategy::Ptr ioStrategy = ISourceScannerStrategy::Ptr(new SourceScannerIoZeraSerial);
    SourceScanner::Ptr scanner = SourceScanner::create(ioDevice, ioStrategy);

    IoQueueEntryList list = ioStrategy->getIoQueueEntriesForScan();
    for(IoQueueEntry::Ptr entry : list) {
        for(int idx = 0; idx<entry->getTransferCount(); ++idx) {
            entry->getTransfer(idx)->getDemoResponder()->activateErrorResponse();
        }
    }

    SourceProperties props(SOURCE_MT_COMMON, "", "");
    QCOMPARE(props.isValid(), true);

    connect(scanner.get(), &SourceScanner::sigScanFinished, [&] (SourceScanner::Ptr scanner){
        props = scanner->getSourcePropertiesFound();
    });

    scanner->startScan();
    QTest::qWait(10);
    QCOMPARE(props.isValid(), false);
}
