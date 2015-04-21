#ifndef SCPIMEASURE_H
#define SCPIMEASURE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

class VeinEntity;

namespace SCPIMODULE
{

class cSCPIMeasure: public QObject
{
    Q_OBJECT

public:
    cSCPIMeasure(VeinEntity *entity);
    ~cSCPIMeasure();

    void execute(quint8 cmd); //

signals:
    void cmdStatus(quint8);
    void cmdAnswer(QString);
    void measContinue();

private:
    VeinEntity* m_pEntity;

    QStateMachine m_MeasureStateMachine;

    QState m_ConfigureState;
    QState m_InitState;
    QFinalState m_FetchState;

    QString m_sAnswer;

    void configuration();

private slots:
    void configure();
    void init();
    void fetch();

    void initDone(const QVariant qvar);
    void initCmdDone(const QVariant qvar);
};


}

#endif // SCPIMEASURE_H
