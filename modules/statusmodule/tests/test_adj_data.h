#ifndef TEST_ADJ_DATA_H
#define TEST_ADJ_DATA_H

#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include <QObject>

class test_adj_data : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void resetAdjustmentDeviceNotAdjusted();
    void resetAdjustmentDeviceAdjusted();
    void resetAdjustmentTwice();

private:
    void prepareEepromTakenFromAdjustedDevice();
    SCPIMODULE::cSCPIModule *getScpiModule();

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_ADJ_DATA_H
