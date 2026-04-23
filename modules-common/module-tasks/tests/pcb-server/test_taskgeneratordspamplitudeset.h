#ifndef TEST_TASKGENERATORDSPAMPLITUDESET_H
#define TEST_TASKGENERATORDSPAMPLITUDESET_H

#include <QObject>

class test_taskgeneratordspamplitudeset : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKGENERATORDSPAMPLITUDESET_H
