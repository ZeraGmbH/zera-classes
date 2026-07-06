#ifndef TEST_SCPI_CMDS_IN_SESSION_H
#define TEST_SCPI_CMDS_IN_SESSION_H

#include "modulemanagertestrunner.h"
#include "scpimodule.h"

class test_scpi_cmds_in_session : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();

    void initialSession();
    void initialTestClient();
    void minScpiDevIface();
    void initialScpiCommandsOnOtherModules();
    void multiReadDoubleDeleteCrasher();
    void devIfaceVeinComponentMultipleEntities();
    void closeSocketOnPendingWriteStbQueryNoCrasher();
    void multilineCommandsLastOpc();
    void catalogFormat();

    void executeRpcQueryWrongRpcName();
    void executeRpcReadLockStateQuery();
    void executeRpcQueryInvalidParams();
    void executeRpcQueryOneParam();
    void doNotExecuteRpcQueryMultipleParams();

private:
    void startModmanWithSession(const QString &sessionFile);
    void killModman();
    SCPIMODULE::cSCPIModule *getScpiModule();
    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_CMDS_IN_SESSION_H
