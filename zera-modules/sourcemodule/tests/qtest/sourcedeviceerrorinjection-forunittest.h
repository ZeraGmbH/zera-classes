#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "source-device/sourcedevice.h"

class SourceDeviceErrorInjection : public ISourceDevice
{
public:
    SourceDeviceErrorInjection(ISourceDevice *sourceUnderTest);

    int startTransaction(IoQueueEntry::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

    void setDemoResonseErrorIdx(int idx);

private:
    int m_simulErrorGroupIdx = -1;
    ISourceDevice *m_sourceUnderTest = nullptr;
};

#endif // SOURCEDEVICEFORUNITTEST_H
