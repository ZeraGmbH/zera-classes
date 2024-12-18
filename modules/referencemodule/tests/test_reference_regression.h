#ifndef TEST_REFERENCE_REGRESSION_H
#define TEST_REFERENCE_REGRESSION_H

#include <QObject>

class test_reference_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_REFERENCE_REGRESSION_H
