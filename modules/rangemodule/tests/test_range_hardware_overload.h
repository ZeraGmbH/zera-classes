#ifndef TEST_RANGE_HARDWARE_OVERLOAD_H
#define TEST_RANGE_HARDWARE_OVERLOAD_H

#include "modulemanagertestrunner.h"

class test_range_hardware_overload : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void noRangeAutoNoRangeGroupingNoActualValues();
    void noRangeAutoNoRangeGroupingWithActualValues();
    void noRangeAutoNoRangeGroupingWithActualValuesChangeRange();

private:
    void fireActualValues(const TestDspInterfacePtr &dspInterfaceProg, double uRms, double iRms, double frequency);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    TestDspInterfacePtr m_sampleDspInterfaceProg;
    TestDspInterfacePtr m_sampleDspInterfaceObserv;
    TestDspInterfacePtr m_sampleDspInterfaceAdj;
};

#endif // TEST_RANGE_HARDWARE_OVERLOAD_H
