#include "sourcedeviceinternal.h"

SourceDeviceInternal::SourceDeviceInternal(IoDeviceBase::Ptr ioDevice, AbstractSourceIoPtr sourceIo) :
    SourceDeviceTemplate(ioDevice, sourceIo)
{
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
