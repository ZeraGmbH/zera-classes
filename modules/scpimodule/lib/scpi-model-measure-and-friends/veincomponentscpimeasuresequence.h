#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include "scpicmdinfo.h"
#include "scpimodeldefinitions.h"
#include "scpitransactionid.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

namespace SCPIMODULE
{

class VeinComponentScpiMeasureSequence : public QObject
{
    Q_OBJECT
public:
    struct Params {
    };
    explicit VeinComponentScpiMeasureSequence(Params params); // not used yet

    static QMultiHash<QString, VeinComponentScpiMeasureSequence *> *getVeinComponentScpiMeasureSequenceStoreSingleton();
    // Moduleinterface initials
    VeinComponentScpiMeasureSequence(const cSCPICmdInfoPtr &scpicmdinfo);
    // Scpi interface copies
    VeinComponentScpiMeasureSequence(const VeinComponentScpiMeasureSequence &obj);
    virtual ~VeinComponentScpiMeasureSequence();

    void receiveMeasureValue(const QVariant &value);
    void execute(ScpiModelTypes modelType, const ScpiTransactionId &scpiTransactionId);
    int entityID();

    static int getInstanceCount();

signals:
    void measContinue();
    void confContinue();
    void readContinue();
    void initContinue();
    void fetchContinue();

    void sigMeasDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void sigConfDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void sigReadDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void sigInitDone(const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);
    void sigFetchDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const SCPIMODULE::VeinComponentScpiMeasureSequence* sender);

private:
    void initialize();
    static QString convertVariantToString(const QVariant &value);

    cSCPICmdInfoPtr m_pSCPICmdInfo;

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

typedef std::shared_ptr<VeinComponentScpiMeasureSequence> VeinComponentScpiMeasureSequencePtr;

}

#endif // SCPIMEASURE_H
