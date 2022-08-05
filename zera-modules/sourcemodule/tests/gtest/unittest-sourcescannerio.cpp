#include <gtest/gtest.h>
#include "source-scanner/sourcescanneriozeraserial.h"
#include "source-scanner/sourcescanneriodemo.h"
#include "io-queue/ioqueuebehaviors.h"
#include <QSet>

TEST(SCANNER_IO_ZERA, LIST_NOT_EMPTY) {
    SourceScannerIoZeraSerial scannerIo;
    EXPECT_GT(scannerIo.getIoQueueEntriesForScan().size(), 0);
}

TEST(SCANNER_IO_ZERA, LIST_ENTRIES_NOT_EMPTY) {
    SourceScannerIoZeraSerial scannerIo;
    for(auto entry : scannerIo.getIoQueueEntriesForScan()) {
        EXPECT_GT(entry->getTransferCount(), 0);
    }
}

struct NameVersion
{
    NameVersion(QByteArray name,
                QByteArray version,
                QByteArray ioName,
                SupportedSourceTypes type) {
        devName = name;
        devVersion = version;
        devIoName = ioName;
        devType = type;
    }
    QByteArray devName;
    QByteArray devVersion;
    QByteArray devIoName;
    SupportedSourceTypes devType;
};

static SourceProperties findSources(NameVersion dev)
{
    // Device response is not an implementation secret: we just read interface
    // docs too...
    QByteArray devResponse = dev.devIoName + " " + dev.devVersion + "\r";
    SourceScannerIoZeraSerial scannerIo;
    IoQueueEntryList transferGroupList = scannerIo.getIoQueueEntriesForScan();

    SourceProperties sourceFound;
    for(auto group : transferGroupList) {
        for(int idx=0; idx<group->getTransferCount(); ++idx) {
            IoTransferDataSingle::Ptr singleTransfer = group->getTransfer(idx);
            singleTransfer->setDataReceived(devResponse); // simulate response
            // act same as IoQueue with IoQueueErrorBehaviors::STOP_ON_FIRST_OK
            if(singleTransfer->didIoPass()) {
                break;
            }
        }
        // evaluate responses
        SourceProperties sourceFoundQuestionMark = scannerIo.evalResponses(group->getGroupId());
        if(sourceFoundQuestionMark.wasSet()) {
            if(sourceFound.wasSet()) {
                // found more than one -> make invalid again
                sourceFound = SourceProperties();
            }
            else {
                sourceFound = sourceFoundQuestionMark;
            }
        }
    }
    return sourceFound;
}

TEST(SCANNER_IO_ZERA, FIND_DEVICES_KNOWN) {
    QList<NameVersion> nameVersion = QList<NameVersion>()
            << NameVersion("MT400", "V1.00", "STSMT400", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT500", "V1.01", "STSMT500", SOURCE_MT_COMMON)
            << NameVersion("MT507", "V1.02", "STSMT507", SOURCE_MT_SINGLE_PHASE_ONLY)
            << NameVersion("MT3000","V1.03", "STSFGMT", SOURCE_MT_COMMON)
            << NameVersion("FG301", "V1.04", "TSFG301", SOURCE_DEMO_FG_4PHASE);

    for(const auto &dev : qAsConst(nameVersion)) {
        SourceProperties sourceFound = findSources(dev);

        EXPECT_TRUE(sourceFound.wasSet());
        EXPECT_EQ(sourceFound.getType(), dev.devType);
        EXPECT_EQ(sourceFound.getName(), dev.devName);
        EXPECT_EQ(sourceFound.getVersion(), dev.devVersion);
    }
}

TEST(SCANNER_IO_ZERA, NO_FIND_ON_UNKNOWN_RESPONSE_LEAD) {
    QList<NameVersion> nameVersion = QList<NameVersion>()
            << NameVersion("MT400", "V1.00", "FOO", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT400", "V1.00", "TSMT", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT400", "V1.00", "STSFG", SOURCE_MT_CURRENT_ONLY);

    for(const auto &dev : qAsConst(nameVersion)) {
        SourceProperties sourceFound = findSources(dev);

        EXPECT_FALSE(sourceFound.wasSet());
    }
}

TEST(SCANNER_IO_DEMO, CREATES_ONE_QUEUE_ENTRY_WITH_ONE_IO) {
    SourceScannerIoDemo scannerIo;
    IoQueueEntryList queueList = scannerIo.getIoQueueEntriesForScan();
    EXPECT_EQ(queueList.size(), 1);
    EXPECT_EQ(queueList[0]->getTransferCount(), 1);
}

TEST(SCANNER_IO_DEMO, GENERATES_INVALID_ON_NO_IO) {
    SourceScannerIoDemo scannerIo;
    IoQueueEntryList queueList = scannerIo.getIoQueueEntriesForScan();
    SourceProperties props = scannerIo.evalResponses(queueList[0]->getGroupId());
    EXPECT_FALSE(props.wasSet());
}

TEST(SCANNER_IO_DEMO, GENERATES_ALL_SOURCE_TYPES) {
    SourceScannerIoDemo scannerIo;

    QSet<SupportedSourceTypes> setTypes;

    for(int idx=0; idx<SOURCE_TYPE_COUNT; ++idx) {
        setTypes.insert(SupportedSourceTypes(idx));
    }
    for(int idx=0; idx<SOURCE_TYPE_COUNT; ++idx) {
        IoQueueEntryList queueList = scannerIo.getIoQueueEntriesForScan();
        queueList[0]->getTransfer(0)->setDataReceived(QByteArray("\r")); // valid response
        SourceProperties props = scannerIo.evalResponses(queueList[0]->getGroupId());
        setTypes.remove(props.getType());
    }
    EXPECT_TRUE(setTypes.isEmpty());

    for(int idx=0; idx<SOURCE_TYPE_COUNT; ++idx) {
        setTypes.insert(SupportedSourceTypes(idx));
    }
    for(int idx=0; idx<SOURCE_TYPE_COUNT; ++idx) {
        IoQueueEntryList queueList = scannerIo.getIoQueueEntriesForScan();
        queueList[0]->getTransfer(0)->setDataReceived(QByteArray("\r"));
        SourceProperties props = scannerIo.evalResponses(queueList[0]->getGroupId());
        setTypes.remove(props.getType());
    }
    EXPECT_TRUE(setTypes.isEmpty());
}
