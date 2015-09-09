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

enum inittype {fromRead, fromInit};

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

    QState m_ConfigureState;
    QState m_InitState;
    QFinalState m_FetchState;

    QString m_sAnswer;

    void configuration();
    QString setAnswer(QVariant qvar);

    int initType;

private slots:
    void configure();
    void init();
    void fetch();
};


}

#endif // SCPIMEASURE_H
