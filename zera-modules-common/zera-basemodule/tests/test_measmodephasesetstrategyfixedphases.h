#ifndef TEST_MEASMODEPHASESETSTRATEGYFIXEDPHASES_H
#define TEST_MEASMODEPHASESETSTRATEGYFIXEDPHASES_H

#include <QObject>

class test_measmodephasesetstrategyfixedphases : public QObject
{
    Q_OBJECT
private slots:
    void canChangeMask();
    void phaseChangeDeny();
    void maskChangeDeny();
    void maskInitialValue();
};

#endif // TEST_MEASMODEPHASESETSTRATEGYFIXEDPHASES_H
