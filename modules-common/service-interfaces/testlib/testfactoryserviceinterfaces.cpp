#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRange(QStringList valueChannelList, bool isReferencce)
{
    Q_UNUSED(isReferencce)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(QStringList valueChannelList)
{
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
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
