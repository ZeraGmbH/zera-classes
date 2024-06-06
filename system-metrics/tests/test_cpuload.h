#ifndef TEST_CPULOAD_H
#define TEST_CPULOAD_H

#include <QObject>

class test_cpuload : public QObject
{
    Q_OBJECT
private slots:
    void test_initial_zero(); // academic?
    void test_user_load();
    void test_nice_load();

    void test_user_load_multiple();
};

#endif // TEST_CPULOAD_H
