#ifndef TEST_SCPI_LEARN_MEASURE_OBJECTS_AND_RELATIONS_H
#define TEST_SCPI_LEARN_MEASURE_OBJECTS_AND_RELATIONS_H

#include <QObject>

class test_scpi_learn_measure_objects_and_relations : public QObject
{
    Q_OBJECT
private slots:
    void countMeasureRelatedObjectsCreated();
    void multipleClientsIndependentStatusbyte();
};

#endif // TEST_SCPI_LEARN_MEASURE_OBJECTS_AND_RELATIONS_H
