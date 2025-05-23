#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include "scpicmdinfo.h"
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
    cSCPIMeasure(std::shared_ptr<QMultiHash<QString, cSCPIMeasure*>> scpiMeasureHash,
                 cSCPICmdInfoPtr scpicmdinfo);
    cSCPIMeasure(const cSCPIMeasure &obj);
    virtual ~cSCPIMeasure();

    void receiveMeasureValue(QVariant qvar);
    void execute(quint8 cmd); //
    int entityID();

    static int getInstanceCount();

signals:
    void measContinue();
    void confContinue();
    void readContinue();
    void initContinue();
    void fetchContinue();

    void sigMeasDone(QString);
    void sigConfDone();
    void sigReadDone(QString);
    void sigInitDone();
    void sigFetchDone(QString);

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
    QString setAnswer(QVariant qvar);
    QList<int> signalList;

    bool m_bInitPending;
    static int m_instanceCount;

private slots:
    void measure();
    void measureConfigure();
    void measureInit();
    void measureFetch();
    void measureDone();

    void configure();
    void configureDone();

    void read();
    void readInit();
    void readFetch();
    void readDone();

    void init();
    void initDone();

    void fetch();
    void fetchSync();
    void fetchFetch();
    void fetchDone();
};

}

#endif // SCPIMEASURE_H
