#ifndef TEST_TOTALMEMORYTRACKER_H
#define TEST_TOTALMEMORYTRACKER_H

#include <QObject>

class test_totalmemorytracker : public QObject
{
    Q_OBJECT
private slots:
    void test_initial_zero();
    void testMemoryUsed();
};

#endif // TEST_TOTALMEMORYTRACKER_H
