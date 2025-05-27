#include "sourcedeviceinternal.h"
#include "sourceswitchjsoninternal.h"

SourceDeviceInternal::SourceDeviceInternal(IoDeviceBase::Ptr ioDevice, const QJsonObject &sourceJsonStruct) :
    SourceDeviceTemplate(ioDevice, sourceJsonStruct)
{
    m_switcher = std::make_unique<SourceSwitchJsonInternal>();
}

void SourceDeviceInternal::setStatusPollTime(int ms)
{
    Q_UNUSED(ms);
}

bool SourceDeviceInternal::close(QUuid uuid)
{
    Q_UNUSED(uuid);
    return false;
}
