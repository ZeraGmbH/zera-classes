#ifndef TEST_RANGE_SCPI_H
#define TEST_RANGE_SCPI_H

#include <QObject>

class test_range_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initialStatesQueries();
};

#endif // TEST_RANGE_SCPI_H
