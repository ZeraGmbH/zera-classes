#ifndef TEST_TASKREFPOWERFETCHCONSTANT_H
#define TEST_TASKREFPOWERFETCHCONSTANT_H

#include <QObject>

class test_taskrefpowerfetchconstant : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsConstantProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKREFPOWERFETCHCONSTANT_H
