#ifndef SOURCEDEVICEFORUNITTEST_H
#define SOURCEDEVICEFORUNITTEST_H

#include "abstractsourceio.h"

class SourceIoErrorInjection : public AbstractSourceIo
{
public:
    SourceIoErrorInjection(AbstractSourceIoPtr sourceUnderTest);

    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    QJsonObject getCapabilities() const override;

    void setDemoResonseErrorIdx(int idx);

private:
    int m_simulErrorGroupIdx = -1;
    AbstractSourceIoPtr m_sourceUnderTest;
};

#endif // SOURCEDEVICEFORUNITTEST_H
