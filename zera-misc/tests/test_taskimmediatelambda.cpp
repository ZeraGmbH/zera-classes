#include "test_taskimmediatelambda.h"
#include "taskimmediatelambda.h"
#include "tasksequence.h"
#include <QTest>

QTEST_MAIN(test_taskimmediatelambda)

void test_taskimmediatelambda::startPass()
{
    TaskCompositePtr task = TaskImmediateLambda::create([&](){
        return true;
    });
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskComposite::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
}

void test_taskimmediatelambda::startFail()
{
    TaskCompositePtr task = TaskImmediateLambda::create([&](){
        return false;
    });
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskComposite::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
}
