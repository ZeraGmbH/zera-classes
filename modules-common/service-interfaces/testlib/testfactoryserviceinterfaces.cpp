#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::getDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    Q_UNUSED(valueChannelList)
    TestDspInterfacePtr dspInterface = std::make_shared<TestDspInterface>();
    m_dspInterfaces.append(dspInterface);
    return dspInterface;
}

DspInterfacePtr TestFactoryServiceInterfaces::getDspInterfaceOther()
{
    // no test specific implementation yet
    return std::make_shared<Zera::cDSPInterface>();
}

const QList<TestDspInterfacePtr> &TestFactoryServiceInterfaces::getInterfaceList() const
{
    return m_dspInterfaces;
}
