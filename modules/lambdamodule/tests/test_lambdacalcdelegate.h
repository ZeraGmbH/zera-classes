#ifndef TEST_LAMBDACALCDELEGATE_H
#define TEST_LAMBDACALCDELEGATE_H

#include <QObject>

class test_lambdacalcdelegate : public QObject
{
    Q_OBJECT
private slots:
    void measModeAvail3LW();
    void measModeNotAvail3LW();
    void measModeAvailPhaseMask110();
    void measModeNotAvailPhaseMask110();

};

#endif // TEST_LAMBDACALCDELEGATE_H
