#ifndef TESTFACTORYSERVICEINTERFACES_H
#define TESTFACTORYSERVICEINTERFACES_H

#include "abstractfactoryserviceinterfaces.h"
#include "testdspinterface.h"
#include <QMap>

enum DspInterfaceCreatedBy {
    MODULEPROG,
    OBSERVER,
    ADJUST
};

enum DspInterfaceInjectableTypes {
    INJECT_NOT_SUPPORTED,

    INJECT_RANGE_OBSERV,
    INJECT_RANGE_ADJUST,
    INJECT_RANGE_PROGRAM,
    INJECT_RMS,
    INJECT_DFT,
    INJECT_FFT
};

class TestFactoryServiceInterfaces : public AbstractFactoryServiceInterfaces
{
public:
    void resetInterfaces() override;
    Zera::DspInterfacePtr createDspInterfaceRangeProg(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeObser(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceRangeAdj(int entityId, QStringList valueChannelList, bool isReference) override;
    Zera::DspInterfacePtr createDspInterfaceDft(int entityId, QStringList valueChannelList, int dftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceFft(int entityId, QStringList valueChannelList, int fftOrder) override;
    Zera::DspInterfacePtr createDspInterfaceRms(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfacePower1(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfacePower2(int entityId, MeasModeSelector* measMode) override;
    Zera::DspInterfacePtr createDspInterfaceThdn(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceOsci(int entityId, QStringList valueChannelList, int interpolation) override;
    Zera::DspInterfacePtr createDspInterfacePeriodAverage(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceMode(int entityId) override;
    Zera::DspInterfacePtr createDspInterfaceRefProg(int entityId, QStringList valueChannelList) override;
    Zera::DspInterfacePtr createDspInterfaceRefAdj(int entityId) override;

    TestDspInterfacePtr getInterface(int entityId, DspInterfaceCreatedBy createdBy);
    TestDspInterfacePtr getInjectableInterface(DspInterfaceInjectableTypes injectType);

private:
    Zera::DspInterfacePtr createDspInterfaceCommon(int entityId,
                                                   DspInterfaceInjectableTypes injectType,
                                                   DspInterfaceCreatedBy createdBy,
                                                   int interruptNoHandled,
                                                   QStringList valueChannelList);
    QMap<int, QMap<DspInterfaceCreatedBy, TestDspInterfacePtr>> m_dspInterfacesByCreatedBy;
    QMap<DspInterfaceInjectableTypes, TestDspInterfacePtr> m_dspInterfacesByInjectType;
};

typedef std::shared_ptr<TestFactoryServiceInterfaces> TestFactoryServiceInterfacesPtr;

#endif // TESTFACTORYSERVICEINTERFACES_H
