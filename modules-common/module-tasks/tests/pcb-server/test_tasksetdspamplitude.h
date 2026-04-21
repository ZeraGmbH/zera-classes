#ifndef TEST_TASKSETDSPAMPLITUDE_H
#define TEST_TASKSETDSPAMPLITUDE_H

#include <QObject>

class test_tasksetdspamplitude : public QObject
{
    Q_OBJECT
private slots:
    void checkScpiSend();
    void returnsNak();
    void timeoutAndErrFunc();
};

#endif // TEST_TASKSETDSPAMPLITUDE_H
