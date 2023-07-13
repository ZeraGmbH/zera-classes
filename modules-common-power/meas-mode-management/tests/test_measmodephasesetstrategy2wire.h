#ifndef TEST_MEASMODEPHASESETSTRATEGY2WIRE_H
#define TEST_MEASMODEPHASESETSTRATEGY2WIRE_H

#include <QObject>

class test_measmodephasesetstrategy2wire : public QObject
{
    Q_OBJECT
private slots:
    void phaseInitial();
    void measSysCount();
    void maxMeasSysCount();
};

#endif // TEST_MEASMODEPHASESETSTRATEGY2WIRE_H
