#ifndef MOVINGWINDOWFILTER_H
#define MOVINGWINDOWFILTER_H

#include <QList>
#include <QVector>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <timertemplateqt.h>

class MovingwindowFilter: public QObject
{
    Q_OBJECT
public:
    MovingwindowFilter(float time = 1.0);
    void setIntegrationTime(float time);

signals:
    void sigActualValues(QVector<float>*);
    void sigNewActualValues();
    void sigFinishFilter();
    void sigTimerInitialized();
    void sigIntegrationTimeExpired();

public slots:
    void receiveActualValues(QVector<float>* actualValues);

protected:
    virtual void addnewValues();
    float m_fintegrationTime;
    TimerTemplateQtPtr m_integrationTimer;
    QList<QVector<double> > m_ActValueFifoList;
    QVector<double> m_FifoSum; // the sums of our actualvalues fifo entries
    QVector<float> m_ActualValues; // our new internal, filtered actualvalues
    QVector<float> *m_pActualValues; // the received actual values

private:
    QStateMachine m_FilterStatemachine;
    QState* m_pactiveState;
    QState* m_pinitFilterState;
    QState* m_psetupFilterState;
    QState* m_pbuildupFilterState;
    QState* m_preadyFilterState;
    QState* m_pdoFilterState;
    QFinalState* m_pFinishState;

private slots:
    void initFilter();
    void setupFilter();
    void buildupFilter();
    void doFilter();
    void stopFilter();
    void restartFilter();
};

#endif // MOVINGWINDOWFILTER_H
