#ifndef TEST_RECORDER_SCPI_H
#define TEST_RECORDER_SCPI_H

#include "modulemanagertestrunner.h"

class test_recorder_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void scpiQueryRunInitial();
    void scpiQueryCountEmpty();
    void scpiWriteCountIgnored();
    void scpiQueryJsonExportEmpty();
    void scpiQueryJsonFireValuesOnce();
    void scpiQueryJsonFireValuesTwice();
    void scpiQueryAll();

private:
    void createModule(int entityId, QMap<QString, QVariant> components);
    void fireActualValues();
    void triggerDftModuleSigMeasuring();
    void createModulesManually();

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
};

#endif // TEST_RECORDER_SCPI_H
