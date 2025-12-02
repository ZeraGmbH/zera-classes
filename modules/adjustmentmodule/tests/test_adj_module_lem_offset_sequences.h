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
    void init();
    void cleanup();

    void adjOffsetActRefZero();
    void adjOffsActRefNonZeroSame();
    void adjOffsActNonZeroRefZero();

    void writeAdjValuesAfterClampInserted();

    void allClampsPermissions_data();
    void allClampsPermissions();
    void internalNoPermission();

private:
    void setupServers();
    void insertClamps(cClamp::ClampTypes clampPhase1To3, cClamp::ClampTypes clampPhaseAux);
    QString clearScpiStatus();
    QString setAutoRangeOff();
    QString setRangeGroupingOff();
    QString setRange(const QString &channelAlias, const QString &rangeName);
    bool setAllRanges(const QString &voltageRangeName, const QString &currrentRangeName);

    QString adjInit(const QString &channelAlias, const QString &rangeName);
    QString adjSendOffset(const QString &channelAlias, const QString &rangeName, const QString& offset);
    QString adjCompute();
    enum CoeffOrNode {
        NODE,
        COEFFICIENT
    };
    QStringList queryAdjNodesOrCoeff(const QString &channelAlias, const QString &rangeName, CoeffOrNode coeffOrNode);
    struct NodeVal
    {
        double sampleOffset = 0.0;
        double takenAt = 0.0;
    };
    QList<NodeVal> decodeNodes(const QStringList nodes);

    static cSenseSettingsPtr getMt310s2dSenseSettings();
    void setLogFileName(const QString &currentTestFunction, const QString &currentDataTag);

    std::unique_ptr<ModuleManagerTestRunner> m_testRunner;
    std::unique_ptr<ScpiModuleClientBlocked> m_scpiClient;
};

#endif // TEST_ADJ_MODULE_LEM_OFFSET_SEQUENCES_H
