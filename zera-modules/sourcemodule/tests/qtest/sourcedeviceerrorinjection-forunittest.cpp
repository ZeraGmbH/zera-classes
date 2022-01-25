#include "sourcedeviceerrorinjection-forunittest.h"

SourceDeviceErrorInjection::SourceDeviceErrorInjection(ISourceDevice *sourceUnderTest) :
    m_sourceUnderTest(sourceUnderTest)
{
    connect(m_sourceUnderTest, &ISourceDevice::sigResponseReceived,
            this, &ISourceDevice::sigResponseReceived);
}

void SourceDeviceErrorInjection::setDemoResonseError(bool active)
{
    m_demoSimulErrorActive = active;
}

int SourceDeviceErrorInjection::startTransaction(IoTransferDataGroup::Ptr transferGroup)
{
    if(m_demoSimulErrorActive) {
        transferGroup->setDemoErrorOnTransfer(0);
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
