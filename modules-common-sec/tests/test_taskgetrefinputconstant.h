#ifndef TEST_TASKGETREFINPUTCONSTANT_H
#define TEST_TASKGETREFINPUTCONSTANT_H

#include <QObject>

class test_taskgetrefinputconstant : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsConstantProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGETREFINPUTCONSTANT_H
