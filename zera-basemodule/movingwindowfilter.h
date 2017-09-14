#ifndef MOVINGWINDOWFILTER_H
#define MOVINGWINDOWFILTER_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QTimer>
#include <QStateMachine>
#include <QState>
#include <QFinalState>


class cMovingwindowFilter: public QObject
{
    Q_OBJECT
public:
    cMovingwindowFilter(float time);

signals:
    void actualValues(QVector<float>*);
    void newActualValues();
    void finishFilter();
    void filterInitialized();

public slots:
    void receiveActualValues(QVector<float>* actualValues);
    void setIntegrationtime(float time);

private:
    float m_fintegrationTime;

    QTimer m_integrationTimer;
    QList<QVector<float> > m_ActValueFifoList;
    QVector<double> m_FifoSum; // the sums of our actualvalues fifo entries
    QVector<float> m_ActualValues; // our new internal, filtered actualvalues
    QVector<float> *m_pActualValues; // the received actual values

    QStateMachine m_FilterStatemachine;
    QState* m_pactiveState;
    QState* m_pinitFilterState;
    QState* m_psetupFilterState;
    QState* m_pbuildupFilterState;
    QState* m_pbuildupFilter2State;
    QState* m_pbuildupFilter3State;
    QState* m_preadyFilterState;
    QState* m_pdoFilterState;
    QFinalState* m_pFinishState;

    void addnewValues();

private slots:
    void initFilter();
    void setupFilter();
    void buildupFilter();
    void buildupFilter2();
    void buildupFilter3();
    void doFilter();
    void stopFilter();
    void restartFilter();
};

#endif // MOVINGWINDOWFILTER_H
