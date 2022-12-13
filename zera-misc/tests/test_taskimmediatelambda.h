#ifndef TEST_TASKIMMEDIATELAMBDA_H
#define TEST_TASKIMMEDIATELAMBDA_H

#include <QObject>

class test_taskimmediatelambda : public QObject
{
    Q_OBJECT
private slots:
    void startPass();
    void startFail();
};

#endif // TEST_TASKIMMEDIATELAMBDA_H
