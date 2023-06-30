#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "sourceio.h"

class SourceIoErrorInjection : public ISourceIo
{
public:
    SourceIoErrorInjection(ISourceIo::Ptr sourceUnderTest);

    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    IoGroupGenerator getIoGroupGenerator() const override;
    SourceProperties getProperties() const override;

    void setDemoResonseErrorIdx(int idx);

private:
    int m_simulErrorGroupIdx = -1;
    ISourceIo::Ptr m_sourceUnderTest;
};

#endif // SOURCEDEVICEFORUNITTEST_H
