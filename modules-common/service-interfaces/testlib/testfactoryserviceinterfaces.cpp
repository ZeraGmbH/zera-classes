#include "testfactoryserviceinterfaces.h"

void TestFactoryServiceInterfaces::resetInterfaces()
{
    m_dspInterfaces.clear();
    m_dspInterfacesTyped.clear();
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                          QStringList valueChannelList,
                                                                          bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                           QStringList valueChannelList,
                                                                           bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    OBSERVER,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                         QStringList valueChannelList,
                                                                         bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    ADJUST,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                    QStringList valueChannelList,
                                                                    int dftOrder)
{
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                    QStringList valueChannelList,
                                                                    int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                    QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                       MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                       MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    7 /* that is what module expects currently */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceSample(int entityId,
                                                                       QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    8 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                     QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                     QStringList valueChannelList,
                                                                     int interpolation)
{
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    6 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                        QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    ADJUST,
                                    0 /* dummy */, {});
}

const QList<TestDspInterfacePtr> &TestFactoryServiceInterfaces::getInterfaceList() const
{
    return m_dspInterfaces;
}

DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceCommon(int entityId,
                                                                       DSPInterfaceType interfaceType,
                                                                       int interruptNoHandled,
                                                                       QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    TestDspInterfacePtr dspInterface = std::make_shared<TestDspInterface>(valueChannelList);
    m_dspInterfaces.append(dspInterface);
    Q_ASSERT(!(m_dspInterfacesTyped.contains(entityId) && m_dspInterfacesTyped[entityId].contains(interfaceType)));
    m_dspInterfacesTyped[entityId][interfaceType] = dspInterface;
    return dspInterface;
}
