#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include "scpimodeldefinitions.h"
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
    void execute(ScpiModelTypes modelType);

    static int getInstanceCount();

signals:
    void measContinue();
    void confContinue();
    void readContinue();
    void initContinue();
    void fetchContinue();

    void sigMeasDone(const QString &scpiResponse);
    void sigConfDone();
    void sigReadDone(const QString &scpiResponse);
    void sigInitDone();
    void sigFetchDone(const QString &scpiResponse);

private:
    void initialize();
    static QString convertVariantToString(const QVariant &value);

    Params m_params;

    QStateMachine m_MeasureStateMachine;
    QStateMachine m_ConfigureStateMachine;
    QStateMachine m_ReadStateMachine;
    QStateMachine m_InitStateMachine;
    QStateMachine m_FetchStateMachine;

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

    bool m_bInitPending = false;
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
