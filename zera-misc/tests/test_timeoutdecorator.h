#ifndef TEST_TIMEOUTDECORATOR_H
#define TEST_TIMEOUTDECORATOR_H

#include <QObject>

class test_timeoutdecorator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void startEmpty();
    void oneOkWithoutDelay();
    void oneErrWithoutDelay();
};

#endif // TEST_TIMEOUTDECORATOR_H
