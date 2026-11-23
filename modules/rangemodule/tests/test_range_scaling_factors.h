#ifndef TEST_RANGE_SCALING_FACTORS_H
#define TEST_RANGE_SCALING_FACTORS_H

#include "testdspinterface.h"
#include "modulemanagertestrunner.h"
#include <memory>

class test_range_scaling_factors : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void changeOneVoltageAndOneCurrentRange();
    void preScalingUOnOff();

private:
    void fireActualValues(const TestDspInterfacePtr &dspInterfaceProg,
                          double uRms, double iRms, double frequency);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    TestDspInterfacePtr m_sampleDspInterfaceProg;
    TestDspInterfacePtr m_sampleDspInterfaceObserv;
    TestDspInterfacePtr m_sampleDspInterfaceAdj;
};

#endif // TEST_RANGE_SCALING_FACTORS_H
