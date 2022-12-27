#include "test_taskimmediatelambda.h"
#include "taskimmediatelambda.h"
#include "tasktesthelper.h"
#include "tasksequence.h"
#include <QTest>

QTEST_MAIN(test_taskimmediatelambda)

void test_taskimmediatelambda::startPass()
{
    TaskCompositePtr task = TaskImmediateLambda::create([&](){
        return true;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskimmediatelambda::startFail()
{
    TaskCompositePtr task = TaskImmediateLambda::create([&](){
        return false;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
