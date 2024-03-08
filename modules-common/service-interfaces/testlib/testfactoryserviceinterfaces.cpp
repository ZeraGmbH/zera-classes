#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReferencce)
{
    Q_UNUSED(isReferencce)
    return createDspInterfaceCommon(interruptNoHandled, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(dftOrder)
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

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int interruptNoHandled, MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(interruptNoHandled, {});
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
