#ifndef TEST_SAMPLE_REGRESSION_H
#define TEST_SAMPLE_REGRESSION_H

#include <QObject>

class test_sample_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_SAMPLE_REGRESSION_H
