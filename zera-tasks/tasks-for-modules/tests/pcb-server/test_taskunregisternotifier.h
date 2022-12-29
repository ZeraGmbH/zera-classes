#ifndef TEST_TASKUNREGISTERNOTIFIER_H
#define TEST_TASKUNREGISTERNOTIFIER_H

#include <QObject>

class test_taskunregisternotifier : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKUNREGISTERNOTIFIER_H
