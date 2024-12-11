#ifndef TEST_SAMPLE_MODULE_REGRESSION_H
#define TEST_SAMPLE_MODULE_REGRESSION_H

#include <QObject>

class test_sample_module_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspCyclicLists();
};

#endif // TEST_SAMPLE_MODULE_REGRESSION_H
