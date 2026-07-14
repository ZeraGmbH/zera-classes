#ifndef TEST_RANGE_SCPI_H
#define TEST_RANGE_SCPI_H

#include "scpimodule.h"
#include "modulemanagertestrunner.h"
#include "scpitestclient.h"

class test_range_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void scpiQueryAndCommand();
    void rangeChangeWithDelay();
    void catalogChangeOnScpiByClamp();
    void catalogChangeOnScpiByClampMultipleClients();

private:
    SCPIMODULE::cSCPIModule *getScpiModule(ModuleManagerTestRunner &testRunner);
    QString sendReceive(SCPIMODULE::ScpiTestClient &client, const QString &scpi, bool removeLineFeedOnReceive = true);
};

#endif // TEST_RANGE_SCPI_H
