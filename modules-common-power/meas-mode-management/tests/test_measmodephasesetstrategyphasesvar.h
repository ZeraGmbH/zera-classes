#ifndef TEST_MEASMODEPHASESETSTRATEGYXWIRE_H
#define TEST_MEASMODEPHASESETSTRATEGYXWIRE_H

#include <QObject>

class test_measmodephasesetstrategyphasesvar : public QObject
{
    Q_OBJECT
private slots:
    void canChangeMask();
    void phaseInitial();
    void maskSet();
};

#endif // TEST_MEASMODEPHASESETSTRATEGYXWIRE_H
