#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>



namespace SCPIMODULE
{

class cSCPIModule;
class cSCPICmdInfo;

class cSCPIMeasure: public QObject
{
    Q_OBJECT

public:
    cSCPIMeasure(cSCPIModule* module, cSCPICmdInfo *scpicmdinfo);
    virtual ~cSCPIMeasure();

    void execute(quint8 cmd); //
    int entityID();

public slots:
    void initDone(const QVariant qvar);

signals:
    void cmdStatus(quint8);
    void cmdAnswer(QString);
    void measContinue();

private:
    cSCPIModule *m_pModule;
    cSCPICmdInfo *m_pSCPICmdInfo;

    QStateMachine m_MeasureStateMachine;
    QStateMachine m_ReadStateMachine;
    QStateMachine m_InitStateMachine;
    QStateMachine m_FetchStateMachine;

    QState m_measConfigureState;
    QState m_measInitState;
    QFinalState m_measFetchState;

    QState m_readInitState;
    QFinalState m_readFetchState;

    QState m_initInitState;
    QFinalState m_initRdyState;

    QState m_fetchWaitInitState;
    QFinalState m_fetchFetchState;

    QString m_sAnswer;

    void configuration();
    QString setAnswer(QVariant qvar);

    bool m_bInitPending;

private slots:
    void configure();
    void init();
    void waitInit();
    void fetch();
};


}

#endif // SCPIMEASURE_H
