#ifndef TEST_MODE_REGRESSION_H
#define TEST_MODE_REGRESSION_H

#include <QObject>

class test_mode_regression : public QObject
{
    Q_OBJECT
private slots:
    void dumpDspSetup();
};

#endif // TEST_MODE_REGRESSION_H
