#ifndef TEST_READINSTRUMENTTYPE_H
#define TEST_READINSTRUMENTTYPE_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QObject>
#include <vf_core_stack_client.h>
#include <vn_networksystem.h>
#include <vn_tcpsystem.h>

class test_readInstrumentType : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void noControllerReadInstrumentType();
    void oneEmobReadInstrumentType();
    void oneMt650ReadInstrumentType();
    void oneEmobOneMt650ReadInstrumentType();
    void TwoEmobsReadInstrumentType();
    void TwoMt650ReadInstrumentType();
    void readHotplugChannelsCom5003();

private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<VeinNet::NetworkSystem> m_netSystem;
    std::unique_ptr<VeinNet::TcpSystem> m_tcpSystem;

    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
    std::unique_ptr<VfCoreStackClient> m_veinClientStack;

};

#endif // TEST_READINSTRUMENTTYPE_H
