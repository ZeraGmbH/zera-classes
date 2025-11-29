#include "testfactoryserviceinterfaces.h"

void TestFactoryServiceInterfaces::resetInterfaces()
{
    m_dspInterfaces.clear();
    m_dspInterfacesByCreatedBy.clear();

    m_dspInterfacesByInjectType.clear();
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                                QStringList valueChannelList,
                                                                                bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_PROGRAM,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                                 QStringList valueChannelList,
                                                                                 bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_OBSERV,
                                    OBSERVER,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                               QStringList valueChannelList,
                                                                               bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_ADJUST,
                                    ADJUST,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                         QStringList valueChannelList,
                                                                         int dftOrder)
{
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(entityId,
                                    INJECT_DFT,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                          QStringList valueChannelList,
                                                                          int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(entityId,
                                    INJECT_FFT,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                          QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_RMS,
                                    MODULEPROG,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    7 /* that is what module expects currently */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceSample(int entityId,
                                                                             QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    8 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                           QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                           QStringList valueChannelList,
                                                                           int interpolation)
{
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    6 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                              QStringList valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    MODULEPROG,
                                    4 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    ADJUST,
                                    0 /* dummy */, {});
}

const QList<TestDspInterfacePtr> &TestFactoryServiceInterfaces::getInterfaceList() const
{
    return m_dspInterfaces;
}

TestDspInterfacePtr TestFactoryServiceInterfaces::getInterface(int entityId, DspInterfaceCreatedBy createdBy)
{
    if (m_dspInterfacesByCreatedBy.contains(entityId) &&
        m_dspInterfacesByCreatedBy[entityId].contains(createdBy))
        return m_dspInterfacesByCreatedBy[entityId][createdBy];
    return nullptr;
}

TestDspInterfacePtr TestFactoryServiceInterfaces::getInjectableInterface(DspInterfaceInjectableTypes injectType)
{
    if (m_dspInterfacesByInjectType.contains(injectType))
        return m_dspInterfacesByInjectType[injectType];
    return nullptr;
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceCommon(int entityId,
                                                                             DspInterfaceInjectableTypes injectType,
                                                                             DspInterfaceCreatedBy createdBy,
                                                                             int interruptNoHandled,
                                                                             QStringList valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    TestDspInterfacePtr dspInterface = std::make_shared<TestDspInterface>(valueChannelList);
    m_dspInterfaces.append(dspInterface);
    Q_ASSERT(!(m_dspInterfacesByCreatedBy.contains(entityId) && m_dspInterfacesByCreatedBy[entityId].contains(createdBy)));
    m_dspInterfacesByCreatedBy[entityId][createdBy] = dspInterface;

    if (injectType != INJECT_NOT_SUPPORTED) {
        Q_ASSERT(!m_dspInterfacesByInjectType.contains(injectType));
        m_dspInterfacesByInjectType[injectType] = dspInterface;
    }
    return dspInterface;
}
