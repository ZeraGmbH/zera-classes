#ifndef MOVINGWINDOWFILTER_H
#define MOVINGWINDOWFILTER_H

#include "timersingleshotqt.h"
#include <QList>
#include <QVector>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

class cMovingwindowFilter: public QObject
{
    Q_OBJECT
public:
    cMovingwindowFilter(float time = 1.0);

signals:
    void actualValues(QVector<float>*);
    void newActualValues();
    void finishFilter();
    void timerInitialized();
    void integrationTimeExpired();

public slots:
    void receiveActualValues(QVector<float>* actualValues);
    void setIntegrationtime(float time);

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
