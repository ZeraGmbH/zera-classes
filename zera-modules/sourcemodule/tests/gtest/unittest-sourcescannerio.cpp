#include <gtest/gtest.h>
#include "source-scanner/sourcescanneriozeraserial.h"
#include "io-queue/ioqueuebehaviors.h"

TEST(SCANNER_ACTION_ZERA, LIST_NOT_EMPTY) {
    SourceScannerIoZeraSerial scannerIo;
    EXPECT_GT(scannerIo.getScanIoGroups().size(), 0);
}

TEST(SCANNER_ACTION_ZERA, LIST_ENTRIES_NOT_EMPTY) {
    SourceScannerIoZeraSerial scannerIo;
    for(auto entry : scannerIo.getScanIoGroups()) {
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
    QList<IoQueueEntry::Ptr> transferGroupList = scannerIo.getScanIoGroups();

    SourceProperties sourceFound;
    for(auto group : transferGroupList) {
        // simulate out/in transactions just by setDataReceived these days
        for(int idx=0; idx<group->getTransferCount(); ++idx) {
            IoTransferDataSingle::Ptr singleTransfer = group->getTransfer(idx);
            singleTransfer->setDataReceived(devResponse); // simulate out/in
            // act same as IoQueue with IoQueueErrorBehaviors::STOP_ON_FIRST_OK
            if(singleTransfer->didIoPass()) {
                break;
            }
        }
        // evaluate responses
        SourceProperties sourceFoundQuestionMark = scannerIo.evalResponses(group->getGroupId());
        if(sourceFoundQuestionMark.isValid()) {
            if(sourceFound.isValid()) {
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

TEST(SCANNER_ACTION_ZERA, FIND_DEVICES_KNOWN) {
    QList<NameVersion> nameVersion = QList<NameVersion>()
            << NameVersion("MT400", "V1.00", "STSMT400", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT500", "V1.01", "STSMT500", SOURCE_MT_COMMON)
            << NameVersion("MT3000","V1.02", "STSFGMT", SOURCE_MT_COMMON)
            << NameVersion("FG301", "V1.03", "TSFG301", SOURCE_DEMO_FG_4PHASE);

    for(auto dev : nameVersion) {
        SourceProperties sourceFound = findSources(dev);

        EXPECT_TRUE(sourceFound.isValid());
        EXPECT_EQ(sourceFound.getType(), dev.devType);
        EXPECT_EQ(sourceFound.getName(), dev.devName);
        EXPECT_EQ(sourceFound.getVersion(), dev.devVersion);
    }
}

TEST(SCANNER_ACTION_ZERA, NO_FIND_ON_UNKNOWN_RESPONSE_LEAD) {
    QList<NameVersion> nameVersion = QList<NameVersion>()
            << NameVersion("MT400", "V1.00", "FOO", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT400", "V1.00", "TSMT", SOURCE_MT_CURRENT_ONLY)
            << NameVersion("MT400", "V1.00", "STSFG", SOURCE_MT_CURRENT_ONLY);

    for(auto dev : nameVersion) {
        SourceProperties sourceFound = findSources(dev);

        EXPECT_FALSE(sourceFound.isValid());
    }
}
