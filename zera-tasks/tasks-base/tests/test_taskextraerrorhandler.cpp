#include "test_taskextraerrorhandler.h"
#include "taskextraerrorhandler.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <timerrunnerfortest.h>
#include <QTest>

QTEST_MAIN(test_taskextraerrorhandler)

void test_taskextraerrorhandler::init()
{
    TaskForTest::resetCounters();
    TimerRunnerForTest::reset();
}

void test_taskextraerrorhandler::startEmpty()
{
    TaskCompositePtr task = TaskExtraErrorHandler::create(nullptr,[&]{});
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskextraerrorhandler::handleError()
{
    int extraErrCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(DEFAULT_EXPIRE, false),[&]{
        extraErrCount++;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(extraErrCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskextraerrorhandler::handleNoError()
{
    int extraErrCount = 0;
    TaskCompositePtr task = TaskExtraErrorHandler::create(TaskForTest::create(DEFAULT_EXPIRE, true),[&]{
        extraErrCount++;
    });
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(extraErrCount, 0);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
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

