#include "sourcedevicebase.h"
#include "json/jsonstructureloader.h"

SourceDeviceBase::SourceDeviceBase(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioDevice(ioDevice)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_ioGroupGenerator = new IoGroupGenerator(paramStructure);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceDeviceBase::onIoGroupFinished);
}

SourceDeviceBase::~SourceDeviceBase()
{
    delete m_ioGroupGenerator;
}

bool SourceDeviceBase::isDemo()
{
    return m_ioDevice->getType() == IoDeviceTypes::DEMO;
}

QString SourceDeviceBase::getIoDeviceInfo()
{
    return m_ioDevice->getDeviceInfo();
}

void SourceDeviceBase::switchState(JsonParamApi state)
{
    m_paramsRequested = state;
    IoQueueEntry::Ptr transferGroup = m_ioGroupGenerator->generateOnOffGroup(m_paramsRequested);
    m_currQueueId.setPending(m_ioQueue.enqueueTransferGroup(transferGroup));
}

void SourceDeviceBase::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent);
}

void SourceDeviceBase::handleSourceCmd(IoQueueEntry::Ptr transferGroup)
{
    if(transferGroup->passedAll()) {
        m_paramsCurrent = m_paramsRequested;
    }
}

void SourceDeviceBase::onIoGroupFinished(IoQueueEntry::Ptr transferGroup)
{
    if(m_currQueueId.isPendingAndRemoveIf(transferGroup->getGroupId())) {
        handleSourceCmd(transferGroup);
    }
}
