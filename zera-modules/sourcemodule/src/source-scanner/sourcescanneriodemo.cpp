#include "sourcescanneriodemo.h"
#include "json/jsonstructapi.h"
#include "json/jsonstructureloader.h"

QList<IoQueueEntry::Ptr> SourceScannerIoDemo::getIoQueueEntriesForScan()
{
    return QList<IoQueueEntry::Ptr>() << IoQueueEntry::Ptr::create(IoQueueErrorBehaviors::STOP_ON_FIRST_OK);
}

SourceProperties SourceScannerIoDemo::evalResponses(int ioGroupId)
{
    Q_UNUSED(ioGroupId)
    SupportedSourceTypes sourceType = getNextSourceType();
    JsonStructApi structureApi = JsonStructApi(JsonStructureLoader::loadJsonDefaultStructure(sourceType));
    return SourceProperties(sourceType, structureApi.getDeviceName(), structureApi.getDeviceVersion());
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
