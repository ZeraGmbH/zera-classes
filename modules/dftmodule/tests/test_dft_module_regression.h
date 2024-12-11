#ifndef TEST_DFT_MODULE_REGRESSION_H
#define TEST_DFT_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"
#include <QObject>

class test_dft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void minimalSession();
    void moduleConfigFromResource();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectActualValuesNoReferenceChannel();
    void injectActualValuesReferenceChannelUL1();
    void injectActualValuesReferenceChannelUL2();
    void injectActualValuesOrder0();
    void injectSymmetricalOrder0();
    void injectSymmetricalOrder1();
    void dumpDspSetup();
private:
    void setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QString channel);
};

#endif // TEST_DFT_MODULE_REGRESSION_H
