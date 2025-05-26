#ifndef TEST_TASKGETINTERNALSOURCECAPABILITIES_H
#define TEST_TASKGETINTERNALSOURCECAPABILITIES_H

#include <QObject>

class test_taskgetinternalsourcecapabilities : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsCapabilitiesProperly();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGETINTERNALSOURCECAPABILITIES_H
