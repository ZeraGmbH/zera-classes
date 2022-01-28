#include "sourcedeviceerrorinjection-forunittest.h"

SourceDeviceErrorInjection::SourceDeviceErrorInjection(ISourceDevice *sourceUnderTest) :
    m_sourceUnderTest(sourceUnderTest)
{
    connect(m_sourceUnderTest, &ISourceDevice::sigResponseReceived,
            this, &ISourceDevice::sigResponseReceived);
}

int SourceDeviceErrorInjection::startTransaction(IoQueueEntry::Ptr transferGroup)
{
    if(m_simulErrorGroupIdx >= 0) {
        IoTransferDataSingle::Ptr ioData = transferGroup->getTransfer(m_simulErrorGroupIdx);
        // no nullptr checks intended: crash in tests is feedback
        ioData->getDemoResponder()->activateErrorResponse();
    }
    return m_sourceUnderTest->startTransaction(transferGroup);
}

IoGroupGenerator SourceDeviceErrorInjection::getIoGroupGenerator() const
{
    return m_sourceUnderTest->getIoGroupGenerator();
}

SourceProperties SourceDeviceErrorInjection::getProperties() const
{
    return m_sourceUnderTest->getProperties();
}

void SourceDeviceErrorInjection::setDemoResonseErrorIdx(int idx)
{
    m_simulErrorGroupIdx = idx;
}
