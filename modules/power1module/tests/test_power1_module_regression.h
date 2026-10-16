#ifndef TEST_POWER1_MODULE_REGRESSION_H
#define TEST_POWER1_MODULE_REGRESSION_H

#include "modulemanagertestrunner.h"

class test_power1_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void minimalSession();
    void veinDumpInitial();
    void injectActualValues();
    void testScpiCommandsDisabled();
    void dumpDspSetup();
    void dumpDspSetupMt310s2Pow1To4();
    void dumpDspSetupCom5003Pow1To4();
    void dumpDspOnMeasModeChange();
    void dumpDspOnMeasModeChangeApparentGeom();
    void dumpDspOnScaleChange();
    void scpiDumpMtPower1Module1();
    void scpiDumpMtPower1Module4(); // ATTOW we are interested in QKHZ / unit
private:
    static void setMeasMode(int entityId,
                            ModuleManagerTestRunner &modman,
                            QString measMode);
    static void setScale(ModuleManagerTestRunner &modman,
                         int uScale,
                         int iScale);
};

#endif // TEST_POWER1_MODULE_REGRESSION_H
