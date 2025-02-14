#include "testfactoryserviceinterfaces.h"

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(QStringList valueChannelList, bool isReference)
{
    Q_UNUSED(isReference)
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

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(7 /* that is what module expects currently */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceSample(QStringList valueChannelList)
{
    return createDspInterfaceCommon(8 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(QStringList valueChannelList)
{
    return createDspInterfaceCommon(4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(QStringList valueChannelList, int interpolation)
{
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(6 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode()
{
    return createDspInterfaceCommon(0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(QStringList valueChannelList)
{
    return createDspInterfaceCommon(4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj()
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
