#include "test_adj_module_lem_offset_sequences.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_adj_module_lem_offset_sequences)

void test_adj_module_lem_offset_sequences::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_adj_module_lem_offset_sequences::cleanup()
{
    m_scpiClient.reset();
    TimeMachineObject::feedEventLoop();
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

void test_adj_module_lem_offset_sequences::dailyOffsetAdjustSequence()
{
    setupServers();
}

void test_adj_module_lem_offset_sequences::writeAdjValuesAfterClampInserted()
{
    setupServers();
}

void test_adj_module_lem_offset_sequences::setupServers()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-dc-session.json");
    m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    insertLemDcClamps();
}

void test_adj_module_lem_offset_sequences::insertLemDcClamps()
{
    QList<AbstractMockAllServices::clampParam> clampParams;
    clampParams.append({"IL1", cClamp::CL1000VDC});
    clampParams.append({"IL2", cClamp::CL1000VDC});
    clampParams.append({"IL3", cClamp::CL1000VDC});
    clampParams.append({"IAUX", cClamp::CL200ADC1000VDC});
    m_testRunner->addClamps(clampParams);
}
