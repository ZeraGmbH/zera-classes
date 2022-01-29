#include "sourcescanneriodemo.h"
#include "json/jsonstructapi.h"
#include "json/jsonstructureloader.h"

SourceScannerIoDemo::SourceScannerIoDemo()
{
    IoQueueEntry::Ptr queueEntry = IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_FIRST_OK);
    QList<IoTransferDataSingle::Ptr> transferList;
    transferList.append(IoTransferDataSingle::Ptr::create("", ""));
    queueEntry->appendTransferList(transferList);
    m_scanIoGroupList.append(queueEntry);
}

IoQueueEntryList SourceScannerIoDemo::getIoQueueEntriesForScan()
{
    return m_scanIoGroupList;
}

SourceProperties SourceScannerIoDemo::evalResponses(int ioGroupId)
{
    SourceProperties props;
    IoQueueEntry::Ptr entry = IoQueueEntryListFind::findGroup(m_scanIoGroupList, ioGroupId);
    if(entry && entry->passedAll()) {
        SupportedSourceTypes sourceType = getNextSourceType();
        JsonStructApi structureApi = JsonStructApi(JsonStructureLoader::loadJsonDefaultStructure(sourceType));
        props = SourceProperties(sourceType, structureApi.getDeviceName(), structureApi.getDeviceVersion());
    }
    return props;
}

SupportedSourceTypes SourceScannerIoDemo::getNextSourceType() {
    static SupportedSourceTypes sDemoTypeCounter(SOURCE_TYPE_COUNT);
    int nextDemoType = sDemoTypeCounter;
    nextDemoType++;
    if(nextDemoType >= SOURCE_TYPE_COUNT) {
        nextDemoType = 0;
    }
    sDemoTypeCounter = SupportedSourceTypes(nextDemoType);
    return sDemoTypeCounter;
}
