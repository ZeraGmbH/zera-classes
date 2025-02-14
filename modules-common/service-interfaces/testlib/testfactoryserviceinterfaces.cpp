#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(entityId)
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder)
{
    Q_UNUSED(entityId)
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int entityId, MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(int entityId, MeasModeSelector *measMode)
{
    Q_UNUSED(entityId)
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(7 /* that is what module expects currently */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceSample(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return createDspInterfaceCommon(8 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return createDspInterfaceCommon(4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation)
{
    Q_UNUSED(entityId)
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(6 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    Q_UNUSED(entityId)
    return createDspInterfaceCommon(0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    return createDspInterfaceCommon(4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    Q_UNUSED(entityId)
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
