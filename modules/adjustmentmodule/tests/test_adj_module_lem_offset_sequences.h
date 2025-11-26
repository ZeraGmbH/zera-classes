#ifndef TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H
#define TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"

class test_adj_module_lem_offset_sequences : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    void dailyOffsetAdjustSequence();
    void writeAdjValuesAfterClampInserted();
private:
    void setupServers();
    void insertLemDcClamps(); // more variants?

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H
