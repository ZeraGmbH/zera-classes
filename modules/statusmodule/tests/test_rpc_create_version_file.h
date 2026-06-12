#ifndef TEST_RPC_CREATE_VERSION_FILE_H
#define TEST_RPC_CREATE_VERSION_FILE_H

#include "modulemanagertestrunner.h"
#include <vf_rpc_invoker.h>
#include <QObject>

class test_rpc_create_version_file : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void createVersionFile();
    void createVersionFileTwice();
private:
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    VfRPCInvokerPtr m_rpcInvoker;
};

#endif // TEST_RPC_CREATE_VERSION_FILE_H
