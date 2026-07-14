#ifndef TEST_SCPI_CMDS_IN_SESSION_H
#define TEST_SCPI_CMDS_IN_SESSION_H

#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpitestclient.h"

class test_scpi_cmds_in_session : public QObject
{
    Q_OBJECT
public: // public for moc
    enum SortTypes {
        SORTED,
        NOT_SORTED
    };
    Q_ENUM(SortTypes)
private slots:
    void initTestCase();
    void initTestCase_data();

    void initialSession();
    void initialTestClient();
    void minScpiDevIface();
    void initialScpiCommandsOnOtherModules();
    void initialScpiCommandsMultipleClients();
    void multiReadDoubleDeleteCrasher();
    void devIfaceVeinComponentMultipleEntities();
    void closeSocketOnPendingWriteStbQueryNoCrasher();
    void multilineCommandsLastOpc();
    void catalogFormat();

    void executeRpcQueryWrongRpcName();
    void executeRpcQueryWrongRpcNameTwoClients();
    void executeRpcReadLockStateQuery();
    void executeRpcReadLockStateQueryTwoClients();
    void executeRpcQueryInvalidParams();
    void executeRpcQueryOneParam();
    void doNotExecuteRpcQueryMultipleParams();

private:
    void startModmanWithSession(const QString &sessionFile);
    void killModman();
    SCPIMODULE::cSCPIModule *getScpiModule();
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi, bool removeLineFeedOnReceive = true);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_SCPI_CMDS_IN_SESSION_H
