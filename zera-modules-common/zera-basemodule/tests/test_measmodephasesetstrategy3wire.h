#ifndef TEST_MEASMODEPHASESETSTRATEGY3WIRE_H
#define TEST_MEASMODEPHASESETSTRATEGY3WIRE_H

#include <QObject>

class test_measmodephasesetstrategy3wire : public QObject
{
    Q_OBJECT
private slots:
    void phaseChangeDeny();
    void maskChangeDeny();
    void maskInitialValue();
};

#endif // TEST_MEASMODEPHASESETSTRATEGY3WIRE_H
