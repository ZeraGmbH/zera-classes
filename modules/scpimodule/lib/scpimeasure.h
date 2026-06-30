#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include "scpicmdinfo.h"
#include "scpitransactionid.h"
#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <memory>

namespace SCPIMODULE
{

enum signalCode {measCont, readCont, initCont, fetchCont};

class cSCPIMeasure : public QObject
{
    Q_OBJECT
public:
    cSCPIMeasure(const std::shared_ptr<QMultiHash<QString, cSCPIMeasure*>> &scpiMeasureHash,
                 const cSCPICmdInfoPtr &scpicmdinfo);
    cSCPIMeasure(const cSCPIMeasure &obj);
    virtual ~cSCPIMeasure();

    void receiveMeasureValue(const QVariant &value);
    void execute(quint8 cmd, const ScpiTransactionId &scpiTransactionId);
    int entityID();

    static int getInstanceCount();

signals:
    void measContinue();
    void confContinue();
    void readContinue();
    void initContinue();
    void fetchContinue();

    void sigMeasDone(const QString& answer, const ScpiTransactionId &scpiTransactionId);
    void sigConfDone(const ScpiTransactionId &scpiTransactionId);
    void sigReadDone(const QString& answer, const ScpiTransactionId &scpiTransactionId);
    void sigInitDone(const ScpiTransactionId &scpiTransactionId);
    void sigFetchDone(const QString& answer, const ScpiTransactionId &scpiTransactionId);

private:
    static QString convertVariantToString(const QVariant &value);
    std::shared_ptr<QMultiHash<QString, cSCPIMeasure*>> m_scpiMeasureHash;
    cSCPICmdInfoPtr m_pSCPICmdInfo;

    void initialize();

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


    QState m_measureState;
    QState m_measureConfigureState;
    QState m_measureInitState;
    QState m_measureFetchState;
    QFinalState m_measureDoneState;

    QState m_confConfigureState;
    QFinalState m_confConfigureDoneState;

    QState m_readState;
    QState m_readInitState;
    QState m_readFetchState;
    QFinalState m_readDoneState;

    QState m_initInitState;
    QFinalState m_initDoneState;

    QState m_fetchState;
    QState m_fetchSyncState;
    QState m_fetchFetchState;
    QFinalState m_fetchDoneState;

    QString m_sAnswer;
    QString setAnswer(const QVariant &qvar);
    QList<int> signalList;

    bool m_bInitPending;
    static int m_instanceCount;

private slots:
    void measure();
    void measureConfigure();
    void measureInit();
    void measureFetch();

    void configure();
    void configureDone();

    void read();
    void readInit();
    void readFetch();

    void init();
    void initDone();

    void fetch();
    void fetchSync();
    void fetchFetch();
};

}

#endif // SCPIMEASURE_H
