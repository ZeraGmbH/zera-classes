#ifndef TEST_TASKRANGEGETADCREJECTION_H
#define TEST_TASKRANGEGETADCREJECTION_H

#include <QObject>

class test_taskrangegetadcrejection : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsRejectionProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKRANGEGETADCREJECTION_H
