#ifndef TEST_TASKSEQUENCE_H
#define TEST_TASKSEQUENCE_H

#include <QObject>

class test_tasksequence : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void startEmpty();
    void oneOk();
    void twoOk();
    void oneError();
    void twoError();
    void threeError();
    void oneErrorOneOk();
    void oneOkOneErrorOneOk();
};

#endif // TEST_TASKSEQUENCE_H
