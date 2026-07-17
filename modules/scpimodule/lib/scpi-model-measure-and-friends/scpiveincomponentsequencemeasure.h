#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include "scpimodeldefinitions.h"
#include "scpitransactionid.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <QJsonObject>
#include <memory>

namespace SCPIMODULE {

class ScpiVeinComponentSequenceMeasure : public QObject
{
    Q_OBJECT
public:
    struct Params {
        const QString scpiModuleName;
        const QString scpiCommand;
        QJsonObject veinComponentInfo;
    };
    ScpiVeinComponentSequenceMeasure(const Params &params);
    ~ScpiVeinComponentSequenceMeasure() override;

    void receiveMeasureValue(const QVariant &value);
    void execute(ScpiModelTypes modelType, const ScpiTransactionId &scpiTransactionId);

    static int getInstanceCount();

signals:
    void measContinue();
    void confContinue();
    void readContinue();
    void initContinue();
    void fetchContinue();

    void sigMeasDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);
    void sigConfDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);
    void sigReadDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);
    void sigInitDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);
    void sigFetchDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::ScpiVeinComponentSequenceMeasure* sender);

private:
    void initialize();
    static QString convertVariantToString(const QVariant &value);

    Params m_params;

    QStateMachine m_MeasureStateMachine;
    QStateMachine m_ConfigureStateMachine;
    QStateMachine m_ReadStateMachine;
    QStateMachine m_InitStateMachine;
    QStateMachine m_FetchStateMachine;

    // State machines cannot perform more than one transaction at a time
    // => keep scpi transaction id per state machine / transaction type for those with answers (measure/read/fetch)
    ScpiTransactionId m_measureScpiTransactionId;
    ScpiTransactionId m_configureScpiTransactionId;
    ScpiTransactionId m_readScpiTransactionId;
    ScpiTransactionId m_initScpiTransactionId;
    ScpiTransactionId m_fetchScpiTransactionId;

    QState m_measureInitState;
    QState m_measureFetchState;
    QFinalState m_measureDoneState;

    QFinalState m_confConfigureDoneState;

    QState m_readInitState;
    QState m_readFetchState;
    QFinalState m_readDoneState;

    QState m_initInitState;
    QFinalState m_initDoneState;

    QState m_fetchSyncState;
    QState m_fetchFetchState;
    QFinalState m_fetchDoneState;

    QString m_sAnswer;
    QString setAnswer(const QVariant &qvar);
    QList<int> signalList;

    bool m_bInitPending;
    static int m_instanceCount;

private slots:
    void measureInit();
    void measureFetch();

    void configureDone();

    void readInit();
    void readFetch();

    void init();
    void initDone();

    void fetchSync();
    void fetchFetch();
};

typedef std::shared_ptr<ScpiVeinComponentSequenceMeasure> VeinComponentScpiMeasureSequencePtr;

}

#endif // SCPIMEASURE_H
