#include "sourcescanneriodemo.h"
#include "jsonstructapi.h"
#include "jsonstructureloader.h"

SourceScannerIoDemo::SourceScannerIoDemo()
{
    IoQueueGroup::Ptr queueEntry = IoQueueGroup::Ptr::create(IoQueueErrorBehaviors::STOP_ON_FIRST_OK);
    QList<IoTransferDataSingle::Ptr> transferList;
    transferList.append(IoTransferDataSingle::Ptr::create("", ""));
    queueEntry->appendTransferList(transferList);
    m_scanIoGroupList.append(queueEntry);
}

IoQueueGroupListPtr SourceScannerIoDemo::getIoQueueGroupsForScan()
{
    return m_scanIoGroupList;
}

SourceProperties SourceScannerIoDemo::evalResponses(int ioGroupId)
{
    SourceProperties props;
    IoQueueGroup::Ptr entry = IoQueueGroupListFind::findGroup(m_scanIoGroupList, ioGroupId);
    if(entry && entry->passedAll()) {
        SupportedSourceTypes sourceType = getNextSourceType();
        JsonStructApi structureApi = JsonStructApi(JsonStructureLoader::loadJsonDefaultStructure(sourceType));
        props = SourceProperties(sourceType,
                                 structureApi.getDeviceName(),
                                 structureApi.getDeviceVersion(),
                                 SourceProtocols::ZERA_SERIAL);
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
