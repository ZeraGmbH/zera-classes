#ifndef TEST_MEASMODEPHASESETSTRATEGY4WIRE_H
#define TEST_MEASMODEPHASESETSTRATEGY4WIRE_H

#include <QObject>

class test_measmodephasesetstrategy4wire : public QObject
{
    Q_OBJECT
private slots:
    void phaseChangeDeny();
    void maskChangeDeny();
    void maskInitialValue();
};

#endif // TEST_MEASMODEPHASESETSTRATEGY4WIRE_H
