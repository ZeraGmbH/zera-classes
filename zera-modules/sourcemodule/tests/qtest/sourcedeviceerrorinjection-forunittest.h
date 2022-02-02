#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "source-device/sourceio.h"

class SourceDeviceErrorInjection : public ISourceIo
{
public:
    SourceDeviceErrorInjection(ISourceIo *sourceUnderTest);

    int startTransaction(IoQueueEntry::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

    void setDemoResonseErrorIdx(int idx);

private:
    int m_simulErrorGroupIdx = -1;
    ISourceIo *m_sourceUnderTest = nullptr;
};

#endif // SOURCEDEVICEFORUNITTEST_H
