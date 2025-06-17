#ifndef TEST_POWER1_MODULE_REGRESSION_H
#define TEST_POWER1_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"

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
    void scpiDumpMtPower1Module1();
    void scpiDumpMtPower1Module4(); // ATTOW we are interested in QKHZ / unit
private:
    void setMeasMode(int entityId,
                     VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                     QString measModeOld, // MUST be correct - otherwise ignored
                     QString measModeNew);
};

#endif // TEST_POWER1_MODULE_REGRESSION_H
