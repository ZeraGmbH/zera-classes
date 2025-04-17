#include "testfactoryserviceinterfaces.h"

void TestFactoryServiceInterfaces::resetInterfaces()
{
    m_dspInterfaces.clear();
    m_dspInterfacesTyped.clear();
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                                QStringList valueChannelList,
                                                                                bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                                 QStringList valueChannelList,
                                                                                 bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    OBSERVER,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                               QStringList valueChannelList,
                                                                               bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    ADJUST,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                         QStringList valueChannelList,
                                                                         int dftOrder)
{
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                          QStringList valueChannelList,
                                                                          int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                          QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    7 /* that is what module expects currently */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceSample(int entityId,
                                                                             QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    8 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                           QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                           QStringList valueChannelList,
                                                                           int interpolation)
{
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    6 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                              QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    ADJUST,
                                    0 /* dummy */, {});
}

const QList<TestDspInterfacePtr> &TestFactoryServiceInterfaces::getInterfaceList() const
{
    return m_dspInterfaces;
}

TestDspInterfacePtr TestFactoryServiceInterfaces::getInterface(int entityId, DSPInterfaceType dspInterfaceType)
{
    if (m_dspInterfacesTyped.contains(entityId) &&
        m_dspInterfacesTyped[entityId].contains(dspInterfaceType))
        return m_dspInterfacesTyped[entityId][dspInterfaceType];
    return nullptr;
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceCommon(int entityId,
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
