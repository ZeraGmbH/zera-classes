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

SourceProperties SourceScannerIoDemo::evalResponses(IoQueueGroup::Ptr ioGroup)
{
    SourceProperties props;
    if(ioGroup && ioGroup->passedAll()) {
        SupportedSourceTypes sourceType = getNextSourceType();
        JsonStructApi structureApi = JsonStructApi(JsonStructureLoader::loadJsonDefaultStructure(sourceType));
        props = SourceProperties(sourceType,
                                 structureApi.getDeviceName(),
                                 structureApi.getDeviceVersion(),
                                 SourceProtocols::ZERA_SERIAL,
                                 SourceProperties::EXTERNAL);
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
