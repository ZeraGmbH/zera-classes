#include "sourcedeviceerrorinjection-forunittest.h"

SourceIoErrorInjection::SourceIoErrorInjection(AbstractSourceIoPtr sourceUnderTest) :
    m_sourceUnderTest(sourceUnderTest)
{
    connect(m_sourceUnderTest.get(), &AbstractSourceIo::sigResponseReceived,
            this, &AbstractSourceIo::sigResponseReceived);
}

int SourceIoErrorInjection::startTransaction(IoQueueGroup::Ptr transferGroup)
{
    if(m_simulErrorGroupIdx >= 0) {
        IoTransferDataSingle::Ptr ioData = transferGroup->getTransfer(m_simulErrorGroupIdx);
        // no nullptr checks intended: crash in tests is feedback
        ioData->getDemoResponder()->activateErrorResponse();
    }
    return m_sourceUnderTest->startTransaction(transferGroup);
}

SourceProperties SourceIoErrorInjection::getProperties() const
{
    return m_sourceUnderTest->getProperties();
}

void SourceIoErrorInjection::setDemoResonseErrorIdx(int idx)
{
    m_simulErrorGroupIdx = idx;
}
