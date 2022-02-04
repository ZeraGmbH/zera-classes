#include "sourcedeviceerrorinjection-forunittest.h"

SourceIoErrorInjection::SourceIoErrorInjection(ISourceIo *sourceUnderTest) :
    m_sourceUnderTest(sourceUnderTest)
{
    connect(m_sourceUnderTest, &ISourceIo::sigResponseReceived,
            this, &ISourceIo::sigResponseReceived);
}

int SourceIoErrorInjection::startTransaction(IoQueueEntry::Ptr transferGroup)
{
    if(m_simulErrorGroupIdx >= 0) {
        IoTransferDataSingle::Ptr ioData = transferGroup->getTransfer(m_simulErrorGroupIdx);
        // no nullptr checks intended: crash in tests is feedback
        ioData->getDemoResponder()->activateErrorResponse();
    }
    return m_sourceUnderTest->startTransaction(transferGroup);
}

IoGroupGenerator SourceIoErrorInjection::getIoGroupGenerator() const
{
    return m_sourceUnderTest->getIoGroupGenerator();
}

SourceProperties SourceIoErrorInjection::getProperties() const
{
    return m_sourceUnderTest->getProperties();
}

void SourceIoErrorInjection::setDemoResonseErrorIdx(int idx)
{
    m_simulErrorGroupIdx = idx;
}
