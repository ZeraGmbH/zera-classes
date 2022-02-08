#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "source-device/sourceio.h"

class SourceIoErrorInjection : public ISourceIo
{
public:
    SourceIoErrorInjection(ISourceIo::Ptr sourceUnderTest);

    int startTransaction(IoQueueEntry::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

    void setDemoResonseErrorIdx(int idx);

private:
    int m_simulErrorGroupIdx = -1;
    ISourceIo::Ptr m_sourceUnderTest;
};

#endif // SOURCEDEVICEFORUNITTEST_H
