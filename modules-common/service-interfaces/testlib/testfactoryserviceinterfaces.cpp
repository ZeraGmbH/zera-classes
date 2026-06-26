#include "testfactoryserviceinterfaces.h"

void TestFactoryServiceInterfaces::resetInterfaces()
{
    m_dspInterfaces.clear();
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeProg(int entityId,
                                                                                const QStringList &valueChannelList,
                                                                                bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_PROGRAM,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeObser(int entityId,
                                                                                 const QStringList &valueChannelList,
                                                                                 bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_OBSERV,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRangeAdj(int entityId,
                                                                               const QStringList &valueChannelList,
                                                                               bool isReference)
{
    Q_UNUSED(isReference)
    return createDspInterfaceCommon(entityId,
                                    INJECT_RANGE_ADJUST,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDft(int entityId,
                                                                         const QStringList &valueChannelList,
                                                                         int dftOrder)
{
    Q_UNUSED(dftOrder)
    return createDspInterfaceCommon(entityId,
                                    INJECT_DFT,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceFft(int entityId,
                                                                          const QStringList &valueChannelList,
                                                                          int fftOrder)
{
    Q_UNUSED(fftOrder)
    return createDspInterfaceCommon(entityId,
                                    INJECT_FFT,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRms(int entityId,
                                                                          const QStringList &valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_RMS,
                                    0 /* dummy */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower1(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePower2(int entityId,
                                                                             MeasModeSelector *measMode)
{
    Q_UNUSED(measMode)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    7 /* that is what module expects currently */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceThdn(int entityId,
                                                                           const QStringList &valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */,
                                    valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceOsci(int entityId,
                                                                           const QStringList &valueChannelList,
                                                                           int interpolation)
{
    Q_UNUSED(interpolation)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */,
                                    valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfacePeriodAverage(int entityId,
                                                                                    const QStringList &valueChannelList,
                                                                                    int maxPeriodCount, int initialPeriodCount)
{
    Q_UNUSED(maxPeriodCount)
    Q_UNUSED(initialPeriodCount)
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */,
                                    valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceMode(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefProg(int entityId,
                                                                              const QStringList &valueChannelList)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_REFERENCE_PROGRAM,
                                    4 /* that is what module expects currently */, valueChannelList);
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceRefAdj(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_REFERENCE_ADJUST,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceStatus(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_NOT_SUPPORTED,
                                    0 /* dummy */, {});
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceDspSuper(int entityId)
{
    return createDspInterfaceCommon(entityId,
                                    INJECT_SUPER,
                                    0 /* dummy */, {});
}

TestDspInterfacePtr TestFactoryServiceInterfaces::findDspInterfaceByType(DspInterfaceInjectableTypes injectType)
{
    for(auto entityIter = m_dspInterfaces.cbegin(); entityIter != m_dspInterfaces.cend(); entityIter++) {
        const QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr> &entityIdInterfaces = entityIter.value();
        if (entityIdInterfaces.contains(injectType))
            return entityIdInterfaces[injectType];
    }
    return nullptr;
}

TestDspInterfacePtr TestFactoryServiceInterfaces::findDspInterfaceByEntityId(int entityId)
{
    auto entityIter = m_dspInterfaces.constFind(entityId);
    if (entityIter != m_dspInterfaces.constEnd()) {
        const QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr> &entityIdInterfaces = entityIter.value();
        if (!entityIdInterfaces.isEmpty())
            return entityIdInterfaces.first();
    }
    return nullptr;
}

QList<TestDspInterfacePtr> TestFactoryServiceInterfaces::findAllDspInterfaceByType(DspInterfaceInjectableTypes injectType)
{
    QList<TestDspInterfacePtr> dspInterfaces;
    for(auto entityIter = m_dspInterfaces.cbegin(); entityIter != m_dspInterfaces.cend(); entityIter++) {
        const QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr> &entityIdInterfaces = entityIter.value();
        if (entityIdInterfaces.contains(injectType))
            dspInterfaces.append(entityIdInterfaces[injectType]);
    }
    return dspInterfaces;
}

QMap<int, QList<TestDspInterfacePtr>> TestFactoryServiceInterfaces::getAllDspInterfaces()
{
    QMap<int, QList<TestDspInterfacePtr>> interfaces;
    for(auto entityIter = m_dspInterfaces.cbegin(); entityIter != m_dspInterfaces.cend(); entityIter++) {
        int entityId = entityIter.key();
        QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr> entityInterfaces = entityIter.value();
        for(auto createdByIter = entityInterfaces.cbegin(); createdByIter != entityInterfaces.cend(); createdByIter++)
            interfaces[entityId].append(createdByIter.value());
    }
    return interfaces;
}

Zera::DspInterfacePtr TestFactoryServiceInterfaces::createDspInterfaceCommon(int entityId,
                                                                             DspInterfaceInjectableTypes injectType,
                                                                             int interruptNoHandled,
                                                                             const QStringList &valueChannelList)
{
    Q_UNUSED(interruptNoHandled)
    TestDspInterfacePtr dspInterface = std::make_shared<TestDspInterface>(valueChannelList, entityId);
    m_dspInterfaces[entityId][injectType] = dspInterface;
    return dspInterface;
}
