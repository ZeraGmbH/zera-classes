#ifndef TEST_MOVINGWINDOWFILTER
#define TEST_MOVINGWINDOWFILTER

#include <QObject>

class test_movingwindowfilter : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void threeSetOfActualValuesWithoutIntegrationTimeElapsed();
private:
    void receiveFilteredValues(QVector<float> *filteredValues);
    QVector<float> m_filteredValues;
};

#endif // TEST_MOVINGWINDOWFILTER
