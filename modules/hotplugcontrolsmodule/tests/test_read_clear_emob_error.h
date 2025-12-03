#ifndef TEST_READ_CLEAR_EMOB_ERROR_H
#define TEST_READ_CLEAR_EMOB_ERROR_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>
#include <testjsonspyeventsystem.h>
#include <vf_core_stack_client.h>
#include <vf_rpc_invoker.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>

class test_read_clear_emob_error : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void readClearErrorNoHotplugScpi();
    void readClearErrorNoParamOneEmobScpi();
    void readClearErrorNoParamOneMt650eScpi();
    void readClearErrorInvalidParamOneEmobOneMt650eScpi();
    void readClearErrorInCorrectParamOneEmobOneMt650eScpi();
    void readClearErrorValidParamOneEmobOneMt650eScpi();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_READ_CLEAR_EMOB_ERROR_H
