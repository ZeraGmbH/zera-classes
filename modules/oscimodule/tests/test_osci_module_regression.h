#ifndef TEST_OSCI_MODULE_REGRESSION_H
#define TEST_OSCI_MODULE_REGRESSION_H

#include "vf_cmd_event_handler_system.h"

class test_osci_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
    void dumpDspSetReference();
private:
         // we expect channel m-Name!!!
    void setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                             QString channelMNameOld, // MUST be correct - otherwise ignored
                             QString channelMNameNew);
};

#endif // TEST_OSCI_MODULE_REGRESSION_H
