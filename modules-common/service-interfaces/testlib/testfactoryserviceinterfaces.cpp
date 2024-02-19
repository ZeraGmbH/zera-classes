#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList)
{
    return createDspInterfaceCommon(interruptNoHandled, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(interruptNoHandled, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    return createDspInterfaceCommon(interruptNoHandled, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOther()
{
    // no test specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}

const QList<TestDspInterfacePtr> &TestFactoryServiceInterfaces::getInterfaceList() const
{
    return m_dspInterfaces;
}

void TestFactoryServiceInterfaces::clearInterfaceList()
{
    m_dspInterfaces.clear();
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceCommon(int interruptNoHandled, QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    TestDspInterfacePtr dspInterface = std::make_shared<TestDspInterface>(valueChannelList);
    m_dspInterfaces.append(dspInterface);
    return dspInterface;
}
