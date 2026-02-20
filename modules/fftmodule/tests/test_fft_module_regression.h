#ifndef TEST_FFT_MODULE_REGRESSION_H
#define TEST_FFT_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"

class test_fft_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void minimalSession();
    void veinDumpInitial();
    void checkActualValueCount();
    void injectValues();
    void dumpDspSetup();
    void dumpDspIL1ReferenceSetup();
    void dumpDspSetReference();
private:
     // we expect channel m-Name!!!
    void setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                             QString channelMNameOld, // MUST be correct - otherwise ignored
                             QString channelMNameNew);
};

#endif // TEST_FFT_MODULE_REGRESSION_H
