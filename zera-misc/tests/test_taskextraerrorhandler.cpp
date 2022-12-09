#include "test_taskextraerrorhandler.h"
#include "taskextraerrorhandler.h"
#include "taskfortest.h"
#include <QTest>

QTEST_MAIN(test_taskextraerrorhandler)

void test_taskextraerrorhandler::startEmpty()
{
    TaskCompositePtr task = TaskExtraErrorHandler::create(nullptr,[&]{});
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

void test_taskextraerrorhandler::handleError()
{
    int extraErrCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(0, false),[&]{
        extraErrCount++;
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
    QCOMPARE(extraErrCount, 1);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
}

void test_taskextraerrorhandler::handleNoError()
{
    int extraErrCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(0, true),[&]{
        extraErrCount++;
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
    QCOMPARE(extraErrCount, 0);
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
}

void test_taskextraerrorhandler::taskId()
{
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(0, true),[&]{});
    int taskId = task->getTaskId();
    int taskIdReceived = 42;
    connect(task.get(), &TaskComposite::sigFinish, [&](bool , int taskId) {
        taskIdReceived = taskId;
    });
    task->start();
    QCOMPARE(taskId, taskIdReceived);
}

