#include "sourcescanneriozeraserial.h"

// some old school local helpers
struct deviceResponseTypePair
{
    deviceResponseTypePair(QByteArray expectedResponse, SupportedSourceTypes sourceType) {
        this->expectedResponse = expectedResponse;
        this->sourceType = sourceType;
    }
    QByteArray expectedResponse;
    SupportedSourceTypes sourceType;
};

struct deviceDetectInfo
{
    deviceDetectInfo(QByteArray queryStr, QList<deviceResponseTypePair> responseTypePairs) {
        this->queryStr = queryStr;
        this->responseTypePairs = responseTypePairs;
    }
    QByteArray queryStr;
    QList<deviceResponseTypePair> responseTypePairs;
};

static QList<deviceDetectInfo> deviceScanListSerial = QList<deviceDetectInfo>()
    << deviceDetectInfo("STS\r", QList<deviceResponseTypePair> ()
                        // Sequence: special -> common
                        << deviceResponseTypePair("STSMT400", SOURCE_MT_CURRENT_ONLY)
                        << deviceResponseTypePair("STSMT551", SOURCE_MT_SINGLE_PHASE_ONLY)
                        << deviceResponseTypePair("STSMT", SOURCE_MT_COMMON)
                        << deviceResponseTypePair("STSFGMT", SOURCE_MT_COMMON))

    // TODO: guessworked initial data
    << deviceDetectInfo("TS\r", QList<deviceResponseTypePair> ()
                        << deviceResponseTypePair("TSFG", SOURCE_DEMO_FG_4PHASE))
       ;


SourceScannerIoZeraSerial::SourceScannerIoZeraSerial()
{
    QList<IoQueueEntry::Ptr> scanIoGroupList;
    scanIoGroupList.append(getCleanupUnfinishedGroup());
    scanIoGroupList.append(getDeviceScanGroup());
    m_scanIoGroupList = scanIoGroupList;
}

QList<IoQueueEntry::Ptr> SourceScannerIoZeraSerial::getIoQueueEntriesForScan()
{
    return m_scanIoGroupList;
}

SourceProperties SourceScannerIoZeraSerial::evalResponses(int ioGroupId)
{
    SourceProperties sourceProperties;
    int iogroupIdx = findGroupIdx(ioGroupId);
    if(iogroupIdx > 0) { // 1st is unfinished cleanup group - see getCleanupUnfinishedGroup
        IoQueueEntry::Ptr groupFound = m_scanIoGroupList[iogroupIdx];
        sourceProperties = evalResponsesForTransactionGroup(groupFound);
    }
    return sourceProperties;
}

IoQueueEntry::Ptr SourceScannerIoZeraSerial::getCleanupUnfinishedGroup()
{
    IoQueueEntry::Ptr ioTermGroup = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    tIoTransferList ioTermList;
    IoTransferDataSingle::Ptr ioTermSingle = IoTransferDataSingle::Ptr::create("\r", "", "");
    ioTermList.append(ioTermSingle);
    ioTermGroup->appendTransferList(ioTermList);
    return ioTermGroup;
}

IoQueueEntry::Ptr SourceScannerIoZeraSerial::getDeviceScanGroup()
{
    IoQueueEntry::Ptr ioScanOutInGroup = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
    tIoTransferList scanList;
    for(auto entry : deviceScanListSerial) {
        QList<QByteArray> expectedLeadList;
        for(auto detectInfoPair : entry.responseTypePairs) {
            expectedLeadList.append(detectInfoPair.expectedResponse);
        }
        IoTransferDataSingle::Ptr ioScanSingle = IoTransferDataSingle::Ptr::create(entry.queryStr, expectedLeadList, "\r");
        scanList.append(ioScanSingle);
    }
    ioScanOutInGroup->appendTransferList(scanList);
    return ioScanOutInGroup;
}

int SourceScannerIoZeraSerial::findGroupIdx(int ioGroupId)
{
    int idxFound = -1;
    for(int idx=0; idx<m_scanIoGroupList.count(); ++idx) {
        if(m_scanIoGroupList[idx]->getGroupId() == ioGroupId) {
            idxFound = idx;
            break;
        }
    }
    return idxFound;
}

QByteArray SourceScannerIoZeraSerial::extractVersionFromResponse(IoTransferDataSingle::Ptr ioData)
{
    QByteArray bytesReceived = ioData->getDataReceived();
    int posV;
    for(posV=bytesReceived.length()-1; posV>=0; --posV) {
        QByteRef curr = bytesReceived[posV];
        if(curr == 'v' || curr == 'V') {
            break;
        }
    }
    QByteArray version;
    if(posV >= 0) {
        QByteArray termList(" \t\n\r");
        for(int pos=posV; pos < bytesReceived.length() && !termList.contains(bytesReceived[pos]); pos++) {
            version.append(bytesReceived[pos]);
        }
    }
    return version;
}

QByteArray SourceScannerIoZeraSerial::extractNameFromResponse(IoTransferDataSingle::Ptr ioData, QByteArray version, QByteArray cmdSend)
{
    QByteArray name = ioData->getDataReceived();
    if(name.startsWith("STSFGMT")) {
        name = "MT3000";
    }
    else {
        // Zera sources echo command in response
        cmdSend.replace("\r" , "");
        name = name.replace(cmdSend, "");
        name = name.replace(version, "");
        name = name.replace(" " , "");
        name = name.replace("\r" , "");
    }
    return name;
}

SourceProperties SourceScannerIoZeraSerial::extractProperties(IoTransferDataSingle::Ptr ioData, int idxInScanList)
{
    int ioPassedIdx = ioData->getPassIdxInExpectedLead();
    deviceDetectInfo detectInfo = deviceScanListSerial[idxInScanList];
    SupportedSourceTypes type = detectInfo.responseTypePairs[ioPassedIdx].sourceType;
    QByteArray sendCmd = detectInfo.queryStr;
    QByteArray version = extractVersionFromResponse(ioData);
    QByteArray name = extractNameFromResponse(ioData, version, sendCmd);
    SourceProperties sourceProperties(type, name, version);
    return sourceProperties;
}

SourceProperties SourceScannerIoZeraSerial::evalResponsesForTransactionGroup(IoQueueEntry::Ptr group)
{
    SourceProperties properties;
    for(int ioIdxInScanGroup=0; ioIdxInScanGroup<group->getTransferCount(); ++ioIdxInScanGroup) {
        IoTransferDataSingle::Ptr singleIo = group->getTransfer(ioIdxInScanGroup);
        if(singleIo->getPassIdxInExpectedLead() >= 0) {
            properties = extractProperties(singleIo, ioIdxInScanGroup);
            if(properties.isValid()) {
                break;
            }
        }
    }
    return properties;
}

