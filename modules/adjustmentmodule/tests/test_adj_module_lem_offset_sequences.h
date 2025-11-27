#ifndef TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H
#define TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H

#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <clamp.h>

class test_adj_module_lem_offset_sequences : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    void dailyOffsetAdjustSequence();
    void writeAdjValuesAfterClampInserted();

    void allClampsPermissions_data();
    void allClampsPermissions();

private:
    void setupServers();
    void insertClamps(cClamp::ClampTypes clampPhase1To3, cClamp::ClampTypes clampPhaseAux);
    bool setAutoRangeOff();
    bool setRangeGroupingOff();
    bool setRange(const QString &channelAlias, const QString &rangeName);
    bool setAllRanges(const QString &voltageRangeName, const QString &currrentRangeName);

    bool adjInit(const QString &channelAlias, const QString &rangeName);
    bool adjSendOffset(const QString &channelAlias, const QString &rangeName, const QString& offset);
    bool adjCompute();

    void setLogFileName(const QString &currentTestFunction, const QString &currentDataTag);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H
