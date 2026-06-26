#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"
#include <QMap>

enum DspInterfaceInjectableTypes {
    INJECT_NOT_SUPPORTED,

    INJECT_RANGE_PROGRAM,
    INJECT_RANGE_OBSERV,
    INJECT_RANGE_ADJUST,
    INJECT_RMS,
    INJECT_DFT,
    INJECT_FFT,
    INJECT_SUPER,
    INJECT_REFERENCE_PROGRAM,
    INJECT_REFERENCE_ADJUST,
};

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    void resetInterfaces() override;
    Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, const QStringList &valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceDft(int entityId, const QStringList &valueChannelList, int dftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceFft(int entityId, const QStringList &valueChannelList, int fftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceRms(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, const QStringList &valueChannelList, int interpolation) override;
    Zera::DspInterfacePtr createDspInterfacePeriodAverage(int entityId, const QStringList &valueChannelList, int maxPeriodCount, int initialPeriodCount) override;
    Zera::DspInterfacePtr createDspInterfaceMode(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, const QStringList &valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceStatus(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceDspSuper(int entityId) override;

    // search criteria ambiguous: return first
    TestDspInterfacePtr findDspInterfaceByType(DspInterfaceInjectableTypes injectType); // TODO return list?
    TestDspInterfacePtr findDspInterfaceByEntityId(int entityId);
    QMap<int, QList<TestDspInterfacePtr>> getAllDspInterfaces();

private:
    Zera::DspInterfacePtr createDspInterfaceCommon(int entityId,
                                                   DspInterfaceInjectableTypes injectType,
                                                   int interruptNoHandled,
                                                   const QStringList &valueChannelList);
    QMap<int, QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr>> m_dspInterfaces;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
