#ifndef TEST_POWER1_MODULE_REGRESSION_H
#define TEST_POWER1_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"

class test_power1_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void veinDumpInitial();
    void injectActualValues();
    void testScpiCommandsDisabled();
    void dumpDspSetup();
    void dumpDspOnMeasModeChange();
    void scpiDumpMtPower1Module1();
    void scpiDumpMtPower1Module4(); // ATTOW we are interested in QKHZ / unit
private:
    void setMeasMode(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                     QString measModeOld, // MUST be correct - otherwise ignored
                     QString measModeNew);
};

#endif // TEST_POWER1_MODULE_REGRESSION_H
