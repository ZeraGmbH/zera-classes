#ifndef TEST_SCPI_CMDS_IN_SESSION_H
#define TEST_SCPI_CMDS_IN_SESSION_H

#include <QObject>
#include <memory>

class test_scpi_cmds_in_session : public QObject
{
    Q_OBJECT
private slots:
    void initialSession();
    void initialTestClient();
    void minScpiDevIface();
    void initialScpiCommandsOnOtherModules();
    void multiReadDoubleDeleteCrasher();
    void devIfaceVeinComponent();
    void devIfaceVeinComponentMultipleEntities();
    void devIfaceVeinComponentMultipleEntitiesForLongXml();
};

#endif // TEST_SCPI_CMDS_IN_SESSION_H
