#ifndef TEST_SCPI_CMDS_IN_SESSION_H
#define TEST_SCPI_CMDS_IN_SESSION_H

#include <QObject>

class test_scpi_cmds_in_session : public QObject
{
    Q_OBJECT
private slots:
    void initialSession();
    void initialTestClient();
    void initialScpiCommandsOnOtherModules();
    void multiReadDoubleDeleteCrasher();
};

#endif // TEST_SCPI_CMDS_IN_SESSION_H
